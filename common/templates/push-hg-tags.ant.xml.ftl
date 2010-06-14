<?xml version="1.0"?>
<project name="SF-TAG-HG-CODE" default="all" xmlns:hlm="http://www.nokia.com/helium">

	<#assign dollar = "$"/>
	
	<#assign target_depends = "" />
	<#assign count = 0 />
	
	<#list data as pkg_detail>
		<target name="sf-push-hg-tag-${count}">
			<!-- Work out the URL of the web *MCL* -->
			<propertyregex property="mcl.${count}" input="${pkg_detail.source}" regexp="(developer.symbian.org)/(oss|sfl)/FCL/(sf|sftools)" casesensitive="false" replace="\1/\2/MCL/\3" defaultValue="${pkg_detail.source}"/>
			<exec executable="hg" dir="${ant['build.drive']}/tagging${pkg_detail.dst}" logError="true">
				<arg value="push"/>
				<arg value="--force"/>
				<arg value="--config"/>
				<arg value="auth.fbf1.prefix=https://developer.symbian.org/"/>
				<arg value="--config"/>
				<arg value="auth.fbf1.username=${dollar}{sf.symbian.account.username}"/>
				<arg value="--config"/>
				<arg value="auth.fbf1.password=${dollar}{sf.symbian.account.password}"/>
				<arg value="--config"/>
				<arg value="auth.fbf2.prefix=http://developer.symbian.org/"/>
				<arg value="--config"/>
				<arg value="auth.fbf2.username=${dollar}{sf.symbian.account.username}"/>
				<arg value="--config"/>
				<arg value="auth.fbf2.password=${dollar}{sf.symbian.account.password}"/>
				<arg value="--rev"/>
				<arg value="TAGS"/>
				<arg value="${dollar}{mcl.${count}}"/>
			</exec>
		</target>
		
		<#if (count > 0) >
			<#assign target_depends ="${target_depends}" + ", "/>
		</#if>
		<#assign target_depends = "${target_depends}" + "sf-push-hg-tag-${count}" />
		
		<#assign count = count + 1 />
	</#list>
	
	<target name="all" depends="${target_depends}"/>

</project>
