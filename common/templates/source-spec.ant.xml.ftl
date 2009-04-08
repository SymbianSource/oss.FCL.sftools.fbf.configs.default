<?xml version="1.0"?>
<project name="SF-SOURCESPEC" default="all" xmlns:hlm="http://www.nokia.com/helium">

<#assign fileset = "" />
<#assign target_depends = "" />
<#assign dollar = "$"/>
<#assign count = 0 />

<!-- remove actual_sources.csv file -->
<delete file="${ant['build.drive']}/output/BOM/sources.csv" quiet="true"/>

<#list data as pkg_detail>
    <target name="sf-prebuild-${count}">
        <#if (count > 0) >
            <#assign fileset = "${fileset}"  />
        </#if>
        <sequential>
            <delete dir="${ant['build.drive']}${pkg_detail.dst}" failonerror="false"/>
            <mkdir dir="${ant['build.drive']}${pkg_detail.dst}"/>
            <hlm:scm verbose="true" scmUrl="scm:hg:${pkg_detail.source}">
                <hlm:checkout basedir="${ant['build.drive']}${pkg_detail.dst}"/>
                <hlm:tags basedir="${ant['build.drive']}${pkg_detail.dst}" reference="hg.tags.id${dollar}{refid}"/>
                <hlm:update basedir="${ant['build.drive']}${pkg_detail.dst}">
                    <hlm:latestTag pattern="${pkg_detail.tag}">
                        <hlm:tagSet refid="hg.tags.id${dollar}{refid}" />
                    </hlm:latestTag>
                </hlm:update>
            </hlm:scm>
            
            <!-- record info on source code repo/rev -->
            <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" outputproperty="sf.sourcesync.${count}.rev">
                <arg value="identify"/>
                <arg value="-i"/>
            </exec>
            <echo message="dir ${ant['build.drive']}${pkg_detail.dst} : revision ${dollar}{sf.sourcesync.${count}.rev}"/>
            <exec executable="cmd" dir="${ant['build.drive']}/output/BOM" output="sources.csv" append="true">
                <arg value="/c"/>
                <arg value="echo"/>
                <arg value="${pkg_detail.source},${pkg_detail.dst},${dollar}{sf.sourcesync.${count}.rev}"/>
            </exec>
        
        </sequential>
    </target>
    <#assign fileset = "${fileset}" + "<fileset dir=\"${ant['build.drive']}${pkg_detail.dst}\" includes=\"${pkg_detail.pattern}\"/>" />
    
    <#if (count == 0) >
    				<#assign target_depends = "${target_depends}" + "sf-prebuild-${count}" />
    </#if>
    <#if (count > 0) >
            <#assign target_depends ="${target_depends}" + ","+"sf-prebuild-${count}"/>
    </#if>
    
    <#assign count = count + 1 />

</#list>

    <path id="system.definition.files">
        <fileset dir="${dollar}{sf.common.config.dir}/sysdefs" includes="*.sysdef.xml"/>
        ${fileset}
    </path>

<target name="all" depends="${target_depends}"/>

</project>