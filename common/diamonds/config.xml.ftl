<?xml version="1.0" encoding="UTF-8"?>
<!-- 
Copyright (c) 2009 Symbian Foundation Ltd
This component and the accompanying materials are made available
under the terms of the License "Eclipse Public License v1.0"
which accompanies this distribution, and is available
at the URL "http://www.eclipse.org/legal/epl-v10.html".

Initial Contributors:
Symbian Foundation Ltd - initial contribution.

Contributors:

Description:
Diamonds configuration template
-->
<configuration>
<config>
    <template-dir path="${ant['helium.dir']}/tools/common/templates/diamonds"/>
    <output-dir path="${ant['build.log.dir']}"/>
    <property name="smtpserver" value="email.smtp.server" />
    <property name="ldapserver" value="email.ldap.server" />
    <property name="initialiser-target-name" value="diamonds" />
    <server>
        <property name="host" value="diamonds.host" />
        <property name="port" value="diamonds.port" />
        <property name="path" value="diamonds.path" />
        <property name="tstampformat" value="yyyy-MM-dd'T'HH:mm:ss" />
        <property name="mail" value="diamonds.mail" />
        <property name="category-property" value="build.family" />
        <property name="buildid-property" value="diamonds.build.id" />
    </server>
</config>
<logger>
    <stages>
        <stage name="pre-build" start="prep" end="prebuild" />
        <stage name="build" start="compile-main" end="compile-main" />
        <stage name="post-build" start="postbuild" end="zip-localised" />
        <stage name="release" start="publish-variants" end="final" />
    </stages>
    <targets>
        <target name="ido-codescanner" template-file="codescanner.xml.ftl"
            logfile="${ant['ido.codescanner.output.dir']}/problemIndex.xml"/>

        <!-- if no logfile provided, looks for xml file to send 
            using <build.id_target_name.xml> file or<target_name.xml> file, 
            if both doesn't exists does nothing. tries to pass ant properties
            and sends it.-->
            
        <target name="create-bom"/>

        <target name="rndsdk-create-api-descr-xml" template-file="apimetrics.xml.ftl"
            logfile="${ant['build.drive']}/output/apidescr/apidescr.xml"/>
            
        <#if (ant?keys?seq_contains('validate.policy.log'))>
        <target name="render-validate-policy" template-file="validate-policy-log.xml.ftl" 
            logfile="${ant['validate.policy.log']}"/>
        </#if>
            
        <!-- SF specific -->
        <target name="sf-diamonds-tag-build"/>
        <target name="sf-diamondize-bom"/>
    </targets>
</logger>
</configuration>