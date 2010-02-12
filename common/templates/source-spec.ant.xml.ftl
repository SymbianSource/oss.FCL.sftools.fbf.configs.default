<?xml version="1.0"?>
<project name="SF-SOURCESPEC" default="all" xmlns:hlm="http://www.nokia.com/helium">

    <!-- Convert \s in cache location, because otherwise they disappear entirely when used in a regex replacement! -->
    <propertyregex property="sf.spec.sourcesync.cachelocation.for.regex" input="${dollar}{sf.spec.sourcesync.cachelocation}" regexp="\\" replace="/" global="true" defaultValue="${dollar}{sf.spec.sourcesync.cachelocation}"/>
    
<#assign fileset = "" />
<#assign sync_list = "" />
<#assign bom_list  = "" />
<#assign change_list  = "" />
<#assign dollar = "$"/>
<#assign count = 0 />

<#list data as csv_file>
  <#list csv_file as pkg_detail>
    <target name="sf-prebuild-${count}">
        
        <!-- Create sf\layer dir on build dir -->
        <mkdir dir="${ant['build.drive']}${pkg_detail.dst}"/>
        <delete dir="${ant['build.drive']}${pkg_detail.dst}" failonerror="true" />
        
        <if>
            <istrue value="${dollar}{sf.spec.sourcesync.usecache}"/>
            <then>
                <!-- Work out cache location from source location -->
                <propertyregex property="sf.spec.sourcesync.cachelocation.${count}" input="${pkg_detail.source}" regexp="^http://developer.symbian.org/" casesensitive="false" replace="${dollar}{sf.spec.sourcesync.cachelocation.for.regex}/Live/"/>
                <propertyregex property="sf.spec.sourcesync.cachelocation.${count}" input="${pkg_detail.source}" regexp="^${ant['sf.spec.sourcesync.local.development.area']}/" casesensitive="false" replace="${dollar}{sf.spec.sourcesync.cachelocation.for.regex}/LocalDev/"/>
            </then>
        </if>
	
        <if>
            <and>
                <isset property="sf.spec.sourcesync.cachelocation.${count}"/>
                <available file="${dollar}{sf.spec.sourcesync.cachelocation.${count}}" type="dir"/>
            </and>
            <then>
                <!-- Package in cache already -->
                <echo message="Pull from ${pkg_detail.source} to ${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                <exec executable="hg" dir="${dollar}{sf.spec.sourcesync.cachelocation.${count}}" failonerror="true">
                    <arg value="pull"/>
                    <arg value="${pkg_detail.source}"/>
                </exec>
                <echo message="Clone from ${dollar}{sf.spec.sourcesync.cachelocation.${count}} to ${ant['build.drive']}${pkg_detail.dst}"/>
                <exec executable="hg" dir="${ant['build.drive']}/" failonerror="true">
                    <arg value="clone"/>
                    <arg value="-U"/>
                    <arg value="--uncompressed"/>
                    <arg value="${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                    <arg value="${ant['build.drive']}${pkg_detail.dst}"/>
                </exec>
                <!-- Update to required revision -->
                <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" failonerror="true">
                    <arg value="update"/>
                    <arg value="-r"/>
                    <arg value="${pkg_detail.pattern}"/>
                </exec>
                <!-- Record the changeset selected, for the BOM -->
                <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" outputproperty="sf.sourcesync.${count}.checksum">
                    <arg value="identify"/>
                    <arg value="-i"/>
                </exec>
            </then>
            <else>
                <echo message="Clone from ${pkg_detail.source} to ${ant['build.drive']}${pkg_detail.dst}"/>
                <exec executable="hg" dir="${ant['build.drive']}/" failonerror="true">
                    <arg value="clone"/>
                    <arg value="-U"/>
                    <arg value="${pkg_detail.source}"/>
                    <arg value="${ant['build.drive']}${pkg_detail.dst}"/>
                </exec>
                <!-- Update to required version -->
                <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" failonerror="true">
                    <arg value="update"/>
                    <arg value="-r"/>
                    <arg value="${pkg_detail.pattern}"/>
                </exec>
                <!-- Record the changeset selected, for the BOM -->
                <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" outputproperty="sf.sourcesync.${count}.checksum">
                    <arg value="identify"/>
                    <arg value="-i"/>
                </exec>
                <if>
                    <isset property="sf.spec.sourcesync.cachelocation.${count}"/>
                    <then>
                        <forget>
                            <nice newpriority="1"/>
                            <!-- Init cache -->
                            <mkdir dir="${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                            <delete dir="${dollar}{sf.spec.sourcesync.cachelocation.${count}}" failonerror="true" />
                            <echo message="Initialise cache at ${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                            <!-- Clone source to get the right default repo -->
                            <exec executable="hg" dir="${ant['build.drive']}/" failonerror="false">
                                <arg value="clone"/>
                                <arg value="-r"/>
                                <arg value="null"/>
                                <arg value="${pkg_detail.source}"/>
                                <arg value="${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                            </exec>
                            <!-- Set the speed-up flag on the cache repo -->
                            <echo file="${dollar}{sf.spec.sourcesync.cachelocation.${count}}/.hg/hgrc" append="true" message="${dollar}{line.separator}"/>
                            <echo file="${dollar}{sf.spec.sourcesync.cachelocation.${count}}/.hg/hgrc" append="true" message="[server]${dollar}{line.separator}"/>
                            <echo file="${dollar}{sf.spec.sourcesync.cachelocation.${count}}/.hg/hgrc" append="true" message="uncompressed=True${dollar}{line.separator}"/>
                            <echo message="Push from ${ant['build.drive']}${pkg_detail.dst} to ${dollar}{sf.spec.sourcesync.cachelocation.${count}} in background"/>
                            <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" failonerror="false">
                                <arg value="push"/>
                                <arg value="-f"/>
                                <arg value="${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                            </exec>
                        </forget>
                    </then>
                </if>
            </else>
        </if>
    </target>
    
    <target name="sf-bom-info-${count}">
        <!-- record info on source code repo/rev in BOM file  -->
        <echo file="${ant['build.drive']}/output/logs/BOM/sources.csv" append="true" message="${pkg_detail.source},${pkg_detail.dst},changeset,${dollar}{sf.sourcesync.${count}.checksum},${pkg_detail.sysdef}${dollar}{line.separator}"/>
    </target>
    
    <target name="sf-bom-change-info-${count}">
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
            <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" output="${ant['build.drive']}/output/logs/BOM/changes.txt" append="true">
                <arg value="log"/>
                <arg value="-r"/>
                <arg value="${dollar}{sf.sourcesync.${count}.checksum}:${dollar}{sf.previous.pdk.tag}"/>
                <#if "${pkg_detail.type}"=="branch">
                <arg value="-b"/>
                <arg value="${pkg_detail.pattern}"/>
                </#if>    
            </exec>
    </target>
    
    <#assign fileset = "${fileset}" + "<fileset dir=\"${ant['build.drive']}${pkg_detail.dst}\" includes=\"${pkg_detail.sysdef}\"/>\r\n        " />       
    <#assign sync_list = "${sync_list}" + "<runtarget target=\"sf-prebuild-${count}\"/>\r\n            "/>
    <#assign bom_list = "${bom_list}" + "<runtarget target=\"sf-bom-info-${count}\"/>\r\n        "/>
    <#assign change_list = "${change_list}" + "<runtarget target=\"sf-bom-change-info-${count}\"/>\r\n        "/>
    <#assign count = count + 1 />
  </#list>
</#list>
    
    <path id="system.definition.files">
        <fileset dir="${dollar}{sf.common.config.dir}/sysdefs" includes="*.sysdef.xml"/>
        ${fileset}
    </path>
    
    <target name="all">
        <parallel threadsPerProcessor="1" failonany="true">
            ${sync_list}
        </parallel>
        
        <echo message="Adding BOM header"/>
        <mkdir dir="${ant['build.drive']}/output/logs/BOM/"/>
        <echo file="${ant['build.drive']}/output/logs/BOM/sources.csv" message="source,dst,type,pattern,sysdef${dollar}{line.separator}"/>
        
        ${bom_list}
        
        <delete file="${ant['build.drive']}/output/logs/BOM/changes.txt" quiet="true"/>
        
        ${change_list}
    </target>
    
</project>
