<?xml version="1.0"?>
<project name="hg-prep" default="all" xmlns:hlm="http://www.nokia.com/helium">
    <property environment="env"/>

    <taskdef name="scm" classname="com.nokia.helium.scm.ant.taskdefs.ScmTask" uri="http://www.nokia.com/helium" />
    <typedef name="latestTag" classname="com.nokia.helium.scm.ant.types.LatestTag" uri="http://www.nokia.com/helium" />
    <typedef name="tagSet" classname="com.nokia.helium.scm.ant.types.TagSet" uri="http://www.nokia.com/helium" />


    <target name="all">
    
    <parallel threadCount="${ant['threads']}">
    <#assign refid=0/>
<#list data as d>
        <sequential>
            <delete dir="${ant['build.drive']}${d.dst}" failonerror="false"/>
            <mkdir dir="${ant['build.drive']}${d.dst}"/>
            <hlm:scm verbose="true" scmUrl="scm:hg:${d.source}">
                <hlm:checkout basedir="${ant['build.drive']}${d.dst}"/>
        
                <hlm:tags basedir="${ant['build.drive']}${d.dst}" reference="hg.tags.id${refid}"/>
        
                <hlm:update basedir="${ant['build.drive']}${d.dst}">
                    <hlm:latestTag pattern="${d.tag}">
                        <hlm:tagSet refid="hg.tags.id${refid}" />
                    </hlm:latestTag>
                </hlm:update>
            </hlm:scm>
        </sequential>
        <#assign refid=refid + 1/>
</#list>
    </parallel>
    </target>
    
    <import file="${ant['helium.dir']}/helium.ant.xml"/>   
</project>
