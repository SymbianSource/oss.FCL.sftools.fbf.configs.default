<?xml version="1.0"?>
<project name="SF-SOURCESPEC" default="all" xmlns:hlm="http://www.nokia.com/helium">

<#assign fileset = "" />
<#assign sync_list = "" />
<#assign bom_list  = "" />
<#assign change_list  = "" />
<#assign dollar = "$"/>
<#assign count = 0 />
<#if ("${ant['sf.spec.sourcesync.archive']}")?? && "${ant['sf.spec.sourcesync.archive']}" == "true">
  <#assign fast_sync = true />
<#else>
  <#assign fast_sync = false />
</#if>
<#if ("${ant['sf.spec.sourcesync.bug419']}")?? && "${ant['sf.spec.sourcesync.bug419']}" == "true">
  <#assign bug419 = true />
<#else>
  <#assign bug419 = false />
</#if>

    <!-- remove previous version of BOM file (if exists)  -->
    <target name="reset-bom-sources-csv">
        <delete file="${ant['build.drive']}/output/logs/BOM/sources.csv" quiet="true"/>
    </target>

<#list data as csv_file>
  <#list csv_file as pkg_detail>
    <target name="sf-prebuild-${count}">
        <sequential>
            <!-- create sf\layer dir  -->
            <mkdir dir="${ant['build.drive']}${pkg_detail.dst}"/>
            <delete dir="${ant['build.drive']}${pkg_detail.dst}" failonerror="false" />
            <!-- Don't use hg archive with tags, as we can have wildcards in the tags... -->
            <#if fast_sync && ("${pkg_detail.type}"!="tag") > 
              <!-- Identify the version on the cache first -->
              <exec executable="hg" dir="${pkg_detail.source}" outputproperty="sf.sourcesync.${count}.checksum">
                  <arg value="identify"/>
                  <arg value="-i"/>
                  <arg value="-r"/>
                  <arg value="${pkg_detail.pattern}"/>
              </exec>
              <!-- hg archive on the version we found -->
              <exec executable="hg" dir="${pkg_detail.source}">
                  <arg value="archive"/>
                  <arg value="-r"/>
                  <arg value="${dollar}{sf.sourcesync.${count}.checksum}"/>
                  <arg value="${ant['build.drive']}${pkg_detail.dst}"/>
              </exec>
            <#else>
            <exec executable="hg" dir="${ant['build.drive']}">
                <arg value="clone"/>
                <arg value="-U"/>
                <arg value="${pkg_detail.source}"/>
                <arg value="${ant['build.drive']}${pkg_detail.dst}"/>
            </exec>
            
            <#if bug419 >
              <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" outputproperty="sf.sourcesync.${count}.checksum">
                  <arg value="identify"/>
                  <arg value="-i"/>
                  <arg value="-r"/>
                  <arg value="${pkg_detail.pattern}"/>
              </exec>
              <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}">
                  <arg value="update"/>
                  <arg value="-r"/>
                  <arg value="${dollar}{sf.sourcesync.${count}.checksum}"/>
              </exec>            
            <#else>
            <hlm:scm verbose="true" scmUrl="scm:hg:${pkg_detail.source}">
                <!--hlm:checkout basedir="${ant['build.drive']}${pkg_detail.dst}"/-->
                <#if "${pkg_detail.type}"=="tag" >
                <hlm:tags basedir="${ant['build.drive']}${pkg_detail.dst}" reference="hg.tags.id${dollar}{refid}"/>
                <hlm:update basedir="${ant['build.drive']}${pkg_detail.dst}">
                <hlm:latestTag pattern="${pkg_detail.pattern}">
                        <hlm:tagSet refid="hg.tags.id${dollar}{refid}" />
                </hlm:latestTag>
                </hlm:update>
                </#if>
                <#if "${pkg_detail.type}"== "changeset" || "${pkg_detail.type}"=="branch">
                <hlm:update basedir="${ant['build.drive']}${pkg_detail.dst}">
                     <hlm:tag name="${pkg_detail.pattern}"/>
                </hlm:update>
                </#if>
            </hlm:scm>
            </#if>
                <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" outputproperty="sf.sourcesync.${count}.checksum">
                <arg value="identify"/>
                <arg value="-i"/>
            </exec>
          </#if>  
        </sequential>
    </target>

    <target name="sf-bom-info-${count}">
        <sequential>
            <!-- record info on source code repo/rev in BOM file  -->
            <echo message="dir ${ant['build.drive']}${pkg_detail.dst} : ${dollar}{sf.sourcesync.${count}.checksum}"/>
            <exec executable="cmd" output="${ant['build.drive']}/output/logs/BOM/sources.csv" append="true">
                <arg value="/c"/>
                <arg value="echo"/>
                <arg value="${pkg_detail.source},${pkg_detail.dst},changeset,${dollar}{sf.sourcesync.${count}.checksum},${pkg_detail.sysdef}"/>
            </exec>
        </sequential>
    </target>

    <target name="sf-bom-change-info-${count}">
        <sequential>
      		<if><not><isset property="sf.sourcesync.${count}.checksum"/></not>
      			<then>
              <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" outputproperty="sf.sourcesync.${count}.checksum">
                <arg value="identify"/>
                <arg value="-i"/>
              </exec>
      			</then>
      		</if>
      		  <echo message="Writing BOM changes since ${dollar}{sf.previous.pdk.tag} for ${pkg_detail.dst}" />
      		  <echo file="${ant['build.drive']}/output/logs/BOM/changes.txt" append="true" message="${dollar}{line.separator}${pkg_detail.source}${dollar}{line.separator}${pkg_detail.dst}${dollar}{line.separator}${dollar}{line.separator}" />
            <#if fast_sync > 
              <exec executable="hg" dir="${pkg_detail.source}" output="${ant['build.drive']}/output/logs/BOM/changes.txt" append="true">
            <#else>      		  
              <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" output="${ant['build.drive']}/output/logs/BOM/changes.txt" append="true">
            </#if>
                <arg value="log"/>
                <arg value="-r"/>
                <arg value="${dollar}{sf.sourcesync.${count}.checksum}:${dollar}{sf.previous.pdk.tag}"/>
                <#if "${pkg_detail.type}"=="branch">
                <arg value="-b"/>
                <arg value="${pkg_detail.pattern}"/>
                </#if>    
            </exec>
          </sequential>
    </target>

    <#assign fileset = "${fileset}" + "<fileset dir=\"${ant['build.drive']}${pkg_detail.dst}\" includes=\"${pkg_detail.sysdef}\"/>\r\n        " />       
    <#assign sync_list = "${sync_list}" + "<runtarget target=\"sf-prebuild-${count}\"/>\r\n    "/>
    <#assign bom_list = "${bom_list}" + "<runtarget target=\"sf-bom-info-${count}\"/>\r\n  "/>
    <#assign change_list = "${change_list}" + "<runtarget target=\"sf-bom-change-info-${count}\"/>\r\n  "/>
    <#assign count = count + 1 />
  </#list>
</#list>

    <path id="system.definition.files">
        <fileset dir="${dollar}{sf.common.config.dir}/sysdefs" includes="*.sysdef.xml"/>
        ${fileset}
    </path>

<target name="all" depends="reset-bom-sources-csv">
  <parallel threadCount="${ant['env.NUMBER_OF_PROCESSORS']}">
    ${sync_list}
  </parallel>

  <echo message="Adding BOM header"/>
      <mkdir dir="${ant['build.drive']}/output/logs/BOM/"/>
      <exec executable="cmd" output="${ant['build.drive']}/output/logs/BOM/sources.csv" append="true">
      <arg value="/c"/>
      <arg value="echo"/>
      <arg value="source,dst,type,pattern,sysdef"/>
  </exec>
  
  ${bom_list}
  <runtarget target="sf-bom-change-info" />
</target>

<target name="sf-bom-change-info">
  <mkdir dir="${ant['build.drive']}/output/logs/BOM/"/>
  <delete file="${ant['build.drive']}/output/logs/BOM/changes.txt" quiet="true"/>

  ${change_list}
</target>

</project>
