<?xml version="1.0"?>
<project name="SF-TRUCLEAN" default="all" xmlns:hlm="http://www.nokia.com/helium">

<#assign target_depends=""/>
<#assign count=0/>
<#assign dollar="$"/>

<#list data as pkg_detail>
  <#if pkg_detail.sysdef != "">
  
<target name="sf-truclean-${count}">
  <sequential>
    <propertyregex override="yes" property="package"  input="${pkg_detail.dst}" regexp=".*sf[\\\/]([^\\^\/]+)[\\\/]([^\\^\/]+)" replace="\1/\2"/>
    <echo message="Calling truclean script for package ${dollar}{package}"/>
      <exec executable="perl" dir="${ant['build.drive']}/" failonerror="false" output="${ant['build.log.dir']}/truclean.log">
        <arg value="${ant['build.drive']}/utilities/uh_parser/truclean.pl"/>
        <arg value="--packageexpr=${dollar}{package}"/>
        <arg value="--releaseablesdir=${ant['build.drive']}/build_info/logs/releaseables"/>
      </exec>
  </sequential>
</target>
  
  <#if (count==0)>
    <#assign target_depends="${target_depends}"+"sf-truclean-${count}"/>
  </#if>
  <#if (count>0)>
    <#assign target_depends="${target_depends}"+","+"sf-truclean-${count}"/>
  </#if>
  <#assign count=count+1/>

  </#if>
</#list>

<target name="all" depends="${target_depends}"/>

</project>