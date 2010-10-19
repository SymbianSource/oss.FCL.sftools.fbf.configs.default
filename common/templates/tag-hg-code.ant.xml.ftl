<?xml version="1.0"?>
<project name="SF-TAG-HG-CODE" default="all" xmlns:hlm="http://www.nokia.com/helium">

	<#assign dollar = "$"/>
	
	<#assign target_depends = "" />
	<#assign count = 0 />
	
	<#list data as pkg_detail>
		<target name="sf-tag-hg-code-${count}">
			<echo message="Cloning TAGS branch from ${pkg_detail.dst}"/>
			<mkdir dir="${ant['build.drive']}/tagging${pkg_detail.dst}"/>
			<exec executable="hg" dir="${ant['build.drive']}/" failonerror="false" resultproperty="clone.from.dst.${count}.exitcode" errorproperty="clone.from.dst.${count}.stderr">
				<arg value="--quiet"/>
				<arg value="clone"/>
				<arg value="--rev"/>
				<arg value="TAGS"/>
				<arg value="${pkg_detail.source}"/>
				<arg value="tagging${pkg_detail.dst}"/>
			</exec>
			<if>
				<not>
					<equals arg1="${dollar}{clone.from.dst.${count}.exitcode}" arg2="0"/>
				</not>
				<then>
					<!-- TAGS branch not in local source - probably caused by package being new and not having a TAGS branch yet -->
					<echo message="Clone failed because '${dollar}{clone.from.dst.${count}.stderr}'"/>
					<!-- Clone the null revision instead -->
					<exec executable="hg" dir="${ant['build.drive']}" failonerror="true">
						<arg value="--quiet"/>
						<arg value="clone"/>
						<arg value="--rev"/>
						<arg value="null"/>
						<arg value="${pkg_detail.dst}"/>
						<arg value="tagging${pkg_detail.dst}"/>
					</exec>
					<!-- Create the branch -->
					<exec executable="hg" dir="${ant['build.drive']}/tagging${pkg_detail.dst}">
						<arg value="branch"/>
						<arg value="TAGS"/>
					</exec>
					<exec executable="hg" dir="${ant['build.drive']}/tagging${pkg_detail.dst}" logError="true">
						<arg value="--quiet"/>
						<arg value="commit"/>
						<arg value="--message"/>
						<arg value="Created TAGS branch"/>
					</exec>
					<!-- Push it back to the source repo -->
					<exec executable="hg" dir="${ant['build.drive']}/tagging${pkg_detail.dst}" logError="true">
						<arg value="--quiet"/>
						<arg value="push"/>
						<arg value="--force"/>
						<arg value="--rev"/>
						<arg value="."/>
					</exec>
				</then>
			</if>
			<!-- Find out what we're synced to in the main source repo -->
			<exec executable="hg" dir="${ant['build.drive']}" outputproperty="hash12.${count}" logError="true">
				<arg value="id"/>
				<arg value="-i"/>
				<arg value="${pkg_detail.dst}"/>
			</exec>
			<!-- Remove any '+' on the end of the hash (indicating that the package overwrote some of its own source files during the build) -->
			<propertyregex property="hash12.${count}" override="true" input="${dollar}{hash12.${count}}" regexp="\+$" replace=""/>
			<!-- Get the full hash id -->
			<exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" outputproperty="hash40.${count}" logError="true">
				<arg value="log"/>
				<arg value="--rev"/>
				<arg value="${dollar}{hash12.${count}}"/>
				<arg value="--template"/>
				<arg value="{node}"/>
			</exec>
			<!-- Manually add the tag to the tags file -->
			<echo message="${dollar}{hash40.${count}} ${ant['sf.tagafterbuild.tag']}${dollar}{line.separator}" file="${ant['build.drive']}/tagging${pkg_detail.dst}/.hgtags" append="true"/>
			<!-- Make sure the file is added (in case this is the first tagging for this package) -->
			<exec executable="hg" dir="${ant['build.drive']}/tagging${pkg_detail.dst}" output="nul:">
				<arg value="add"/>
				<arg value=".hgtags"/>
			</exec>
			<!-- Manually commit it -->
			<exec executable="hg" dir="${ant['build.drive']}/tagging${pkg_detail.dst}" logError="true">
				<arg value="--quiet"/>
				<arg value="commit"/>
				<arg value="--user"/>
				<arg value="${dollar}{sf.hg.ui.username}"/>
				<arg value="--message"/>
				<arg value="Added tag ${ant['sf.tagafterbuild.tag']} for changeset ${dollar}{hash12.${count}}"/>
			</exec>
		</target>
		
		<#if (count > 0) >
			<#assign target_depends ="${target_depends}" + ", "/>
		</#if>
		<#assign target_depends = "${target_depends}" + "sf-tag-hg-code-${count}" />
		
		<#assign count = count + 1 />
	</#list>
	
	<target name="all" depends="${target_depends}"/>

</project>
