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
            <hlm:scm verbose="true" scmUrl="scm:hg:${ant['build.drive']}${d.dst}">
                <hlm:tag basedir="${ant['build.drive']}${d.dst}" name="${ant['build.id']}"/>
            </hlm:scm>
        </sequential>
        <#assign refid=refid + 1/>
</#list>
    </parallel>
    </target>
    
    <import file="${ant['helium.dir']}/helium.ant.xml"/>   
</project>
