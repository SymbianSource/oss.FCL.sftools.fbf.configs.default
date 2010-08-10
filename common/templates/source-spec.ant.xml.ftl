<?xml version="1.0"?>
<project name="SF-SOURCESPEC" default="all" xmlns:hlm="http://www.nokia.com/helium">

<#assign dollar = "$"/>

    <import file="${dollar}{sf.common.config.dir}/build.retry.xml"/>

    <!-- Convert \s in cache location, because otherwise they disappear entirely when used in a regex replacement! -->
    <propertyregex property="sf.spec.sourcesync.cachelocation.for.regex" input="${dollar}{sf.spec.sourcesync.cachelocation}" regexp="\\" replace="/" global="true" defaultValue="${dollar}{sf.spec.sourcesync.cachelocation}"/>

<#assign fileset = "" />
<#assign sync_list = "" />
<#assign bom_list  = "" />
<#assign change_list  = "" />
<#assign count = 0 />

<#list data as csv_file>
  <#list csv_file as pkg_detail>
    <target name="sf-prebuild-${count}">
        
        <!-- if defined the revision override take that as revision and change the url to point to FCL -->
        <if>
            <not><equals arg1="${ant['sf.spec.sources.revision']}" arg2="" trim="true"/></not>
            <then>
                <property name="sources.${count}.revision" value="${ant['sf.spec.sources.revision']}"/>
                <propertyregex property="sources.${count}.URL" input="${pkg_detail.source}" regexp="/MCL/" casesensitive="false" replace="/FCL/" defaultValue="${pkg_detail.source}"/>
            </then>
            <else>
                <property name="sources.${count}.revision" value="${pkg_detail.pattern}"/>
                <property name="sources.${count}.URL" value="${pkg_detail.source}"/>
            </else>
        </if>
        
        <!-- Create sf\layer dir on build dir -->
        <mkdir dir="${ant['build.drive']}${pkg_detail.dst}"/>
        <delete dir="${ant['build.drive']}${pkg_detail.dst}" failonerror="true" />
        
        <if>
            <istrue value="${dollar}{sf.spec.sourcesync.usecache}"/>
            <then>
                <!-- Work out cache location from source location -->
                <propertyregex property="sf.spec.sourcesync.cachelocation.${count}" input="${dollar}{sources.${count}.URL}" regexp="^http://developer.symbian.org/" casesensitive="false" replace="${dollar}{sf.spec.sourcesync.cachelocation.for.regex}/Live/"/>
                <propertyregex property="sf.spec.sourcesync.cachelocation.${count}" input="${dollar}{sources.${count}.URL}" regexp="^${ant['sf.spec.sourcesync.local.development.area']}/" casesensitive="false" replace="${dollar}{sf.spec.sourcesync.cachelocation.for.regex}/LocalDev/"/>
            </then>
        </if>
        
        <!-- Convert source tag/branch to to changeset hash, in case it's a local tag on the server -->
        <retry tries="10" uniquename="${count}" failonerror="false">
            <sequential>
                <exec executable="hg" failonerror="true" output="${ant['temp.build.dir']}/sf.sourcesync.${count}.checksum" error="nul:">
                    <arg value="id"/>
                    <arg value="${dollar}{sources.${count}.URL}"/>
                    <arg value="-r"/>
                    <arg value="${dollar}{sources.${count}.revision}"/>
                    <arg value="-q"/>
                </exec>
                <loadfile property="sf.sourcesync.${count}.checksum" srcFile="${ant['temp.build.dir']}/sf.sourcesync.${count}.checksum"/>
                <propertyregex property="sf.sourcesync.${count}.checksum" override="true" input="${dollar}{sf.sourcesync.${count}.checksum}" regexp="(\S{12})" select="\1"/>
            </sequential>
        </retry>
        <fail unless="sf.sourcesync.${count}.checksum" message="Failed to get changeset ID for revision '${dollar}{sources.${count}.revision}' for repo ${dollar}{sources.${count}.URL}"/>

        <if>
            <and>
                <isset property="sf.spec.sourcesync.cachelocation.${count}"/>
                <available file="${dollar}{sf.spec.sourcesync.cachelocation.${count}}" type="dir"/>
            </and>
            <then>
                <!-- Package in cache already -->
                <retry tries="3" uniquename="${count}" failonerror="0">
                    <sequential>
                        <echo message="Pull from ${dollar}{sources.${count}.URL} to ${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                        <exec executable="hg" dir="${dollar}{sf.spec.sourcesync.cachelocation.${count}}" failonerror="true">
                            <arg value="pull"/>
                            <arg value="${dollar}{sources.${count}.URL}"/>
                        </exec>
                        <property name="sf.spec.sourcesync.cache.pull.succeeded.${count}" value="1"/>
                    </sequential>
                </retry>
                <if>
                    <isset property="sf.spec.sourcesync.cache.pull.succeeded.${count}"/>
                    <then>
                        <echo message="Clone from ${dollar}{sf.spec.sourcesync.cachelocation.${count}} to ${ant['build.drive']}${pkg_detail.dst}"/>
                        <exec executable="hg" dir="${ant['build.drive']}/" failonerror="true">
                            <arg value="clone"/>
                            <arg value="-U"/>
                            <arg value="--uncompressed"/>
                            <arg value="${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                            <arg value="${ant['build.drive']}${pkg_detail.dst}"/>
                        </exec>
                    </then>
                    <else>
                        <!-- Uh oh - the cache is corrupted somehow -->
                        <!-- (Try to) recover the cache repo -->
                        <forget>
                            <exec executable="hg" dir="${dollar}{sf.spec.sourcesync.cachelocation.${count}}" failonerror="false" resultproperty="sf.spec.sourcesync.cache.recover.error.code.${count}">
                                <arg value="recover"/>
                            </exec>
                            <if>
                                <not><equals arg1="0" arg2="${dollar}{sf.spec.sourcesync.cache.recover.error.code.${count}}"/></not>
                                <then>
                                    <echo message="Trashing ${dollar}{sf.spec.sourcesync.cachelocation.${count}} as broken"/>
                                    <delete dir="${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                                </then>
                            </if>
                        </forget>
                        <!-- In the meantime, by-pass it for this build -->
                        <retry tries="30" uniquename="${count}" failonerror="false">
                            <sequential>
                                <echo message="Clone from ${dollar}{sources.${count}.URL} to ${ant['build.drive']}${pkg_detail.dst}"/>
                                <exec executable="hg" dir="${ant['build.drive']}/" failonerror="true">
                                    <arg value="clone"/>
                                    <arg value="-U"/>
                                    <arg value="${dollar}{sources.${count}.URL}"/>
                                    <arg value="${ant['build.drive']}${pkg_detail.dst}"/>
                                </exec>
                                <property name="sf.spec.sourcesync.cache.clone.succeeded.${count}" value="1"/>
                            </sequential>
                            <cleanup>
                                <delete dir="${ant['build.drive']}${pkg_detail.dst}"/>
                            </cleanup>
                        </retry>
                        <fail unless="sf.spec.sourcesync.cache.clone.succeeded.${count}" message="ERROR: Failed to clone ${dollar}{sources.${count}.URL} to ${ant['build.drive']}${pkg_detail.dst}"/>
                    </else>
                </if>
                <!-- Update to required revision -->
                <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" failonerror="true">
                    <arg value="update"/>
                    <arg value="-r"/>
                    <arg value="${dollar}{sf.sourcesync.${count}.checksum}"/>
                </exec>
            </then>
            <else>
                <!-- Package not in cache, or cache not in use -->
                <retry tries="10" uniquename="${count}" failonerror="false">
                    <sequential>
                        <echo message="Clone from ${dollar}{sources.${count}.URL} to ${ant['build.drive']}${pkg_detail.dst}"/>
                        <exec executable="hg" dir="${ant['build.drive']}/" failonerror="true">
                            <arg value="clone"/>
                            <arg value="-U"/>
                            <arg value="${dollar}{sources.${count}.URL}"/>
                            <arg value="${ant['build.drive']}${pkg_detail.dst}"/>
                        </exec>
                        <property name="sf.spec.sourcesync.cache.clone.succeeded.${count}" value="1"/>
                    </sequential>
                    <cleanup>
                        <delete dir="${ant['build.drive']}${pkg_detail.dst}"/>
                    </cleanup>
                </retry>
                <fail unless="sf.spec.sourcesync.cache.clone.succeeded.${count}" message="ERROR: Failed to clone ${dollar}{sources.${count}.URL} to ${ant['build.drive']}${pkg_detail.dst}"/>
                <!-- Update to required version -->
                <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" failonerror="true">
                    <arg value="update"/>
                    <arg value="-r"/>
                    <arg value="${dollar}{sf.sourcesync.${count}.checksum}"/>
                </exec>
                <if>
                    <isset property="sf.spec.sourcesync.cachelocation.${count}"/>
                    <then>
                        <!-- Init cache -->
                        <mkdir dir="${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                        <delete dir="${dollar}{sf.spec.sourcesync.cachelocation.${count}}" failonerror="true" />
                        <echo message="Initialise cache at ${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                        <!-- Initialise an empty repo -->
                        <exec executable="hg" dir="${ant['build.drive']}/" failonerror="false">
                            <arg value="init"/>
                            <arg value="${dollar}{sf.spec.sourcesync.cachelocation.${count}}"/>
                        </exec>
                        <!-- Configure the paths and the speed-up flag on the cache repo -->
                        <echo file="${dollar}{sf.spec.sourcesync.cachelocation.${count}}/.hg/hgrc" append="false" message="[paths]${dollar}{line.separator}"/>
                        <echo file="${dollar}{sf.spec.sourcesync.cachelocation.${count}}/.hg/hgrc" append="true" message="default = ${dollar}{sources.${count}.URL}${dollar}{line.separator}"/>
                        <echo file="${dollar}{sf.spec.sourcesync.cachelocation.${count}}/.hg/hgrc" append="true" message="${dollar}{line.separator}"/>
                        <echo file="${dollar}{sf.spec.sourcesync.cachelocation.${count}}/.hg/hgrc" append="true" message="[server]${dollar}{line.separator}"/>
                        <echo file="${dollar}{sf.spec.sourcesync.cachelocation.${count}}/.hg/hgrc" append="true" message="uncompressed=True${dollar}{line.separator}"/>
                        <echo message="(Push from ${ant['build.drive']}${pkg_detail.dst} to ${dollar}{sf.spec.sourcesync.cachelocation.${count}} in background)"/>
                        <forget>
                            <nice newpriority="1"/>
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
        <!-- record info on source code repo/rev in BOM file and in BIT file -->
        <echo file="${ant['build.drive']}/output/logs/BOM/sources.csv" append="true" message="${dollar}{sources.${count}.URL},${pkg_detail.dst},changeset,${dollar}{sf.sourcesync.${count}.checksum},${pkg_detail.sysdef}${dollar}{line.separator}"/>
        <echo message="content&#x0009;sources,${dollar}{sources.${count}.URL},${dollar}{sf.sourcesync.${count}.checksum}&#x000d;&#x000a;" file="${ant['build.log.dir']}/bitinfo.txt" append="true"/>
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
        <echo file="${ant['build.drive']}/output/logs/BOM/changes.txt" append="true" message="${dollar}{line.separator}${dollar}{sources.${count}.URL}${dollar}{line.separator}${pkg_detail.dst}${dollar}{line.separator}${dollar}{line.separator}" />
            <exec executable="hg" dir="${ant['build.drive']}${pkg_detail.dst}" output="${ant['build.drive']}/output/logs/BOM/changes.txt" append="true">
                <arg value="log"/>
                <arg value="-r"/>
                <arg value="${dollar}{sf.sourcesync.${count}.checksum}:${dollar}{sf.previous.pdk.tag}"/>
                <arg value="-f"/>
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
