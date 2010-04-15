<?xml version="1.0"?>
<project name="run-qmake-${ant['sysdef.configuration']}" default="all">

    <target name="all">
        <parallel threadCount="${r'$'}{number.of.threads}">
    <#list data["//unit/@proFile/.."] as unit>
            <sequential>
                <echo>Running qmake for ${unit.@bldFile}/${unit.@proFile}</echo>
                <if>
                    <available file="${r'$'}{build.drive}/${unit.@bldFile}" type="dir"/>
                    <then>
                        <exec executable="cmd" dir="${r'$'}{build.drive}/${unit.@bldFile}" failonerror="false">
                            <arg value="/C"/>
                            <arg value="qmake"/>
                            <!-- arg value="-listgen"/ -->
                            <#if unit.@qmakeArgs[0]??>
                            <arg line="${unit.@qmakeArgs?xml}"/>
                            <#else>
                            <arg line="${ant['qt.qmake.default.args']?xml}"/>
                            </#if>
                            <arg value="${unit.@proFile?xml}"/>
                        </exec>
                    </then>
                    <else>
                       <echo message="ERROR: Directory ${r'$'}{build.drive}/${unit.@bldFile} doesn't exist."/>
                    </else>
                </if>
            </sequential>
    </#list>
        </parallel>
    </target>
    
</project>
