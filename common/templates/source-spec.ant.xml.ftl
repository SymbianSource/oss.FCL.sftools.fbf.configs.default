<?xml version="1.0"?>
<project name="SF-SOURCESPEC" default="all" xmlns:hlm="http://www.nokia.com/helium">

<#assign fileset = "" />
<#assign target_depends = "" />
<#assign dollar = "$"/>
<#assign count = 0 />

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
                <exec command="hg identify -n" dir="${ant['build.drive']}${pkg_detail.dst}" outputproperty="sf.sourcesync.repo.n"/>
                <exec command="hg identify -i" dir="${ant['build.drive']}${pkg_detail.dst}" outputproperty="sf.sourcesync.repo.i"/>
                <echo message="dir ${ant['build.drive']}${pkg_detail.dst} : revision ${sf.sourcesync.repo.n}:${sf.sourcesync.repo.i}"/>
            </hlm:scm>
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