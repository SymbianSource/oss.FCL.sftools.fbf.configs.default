<?xml version="1.0"?>
<project name="run-qmake-${ant['sysdef.configuration']}" default="all">

    <target name="all">
    <if><available file="${r'$'}{build.drive}/epoc32/tools/qt/qmake.exe" type="file"/>
    <then>

        <parallel threadCount="${r'$'}{number.of.threads}">
    <#list data["//unit/@proFile/.."] as unit>
            <sequential>

          <#if (unit.@proFile!="hb.pro")>

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
          <#else>

                <echo>Running configure.py for ${unit.@bldFile}/${unit.@proFile}</echo>
                <if>
                    <available file="${r'$'}{build.drive}/${unit.@bldFile}" type="dir"/>
                    <then>
                        <exec executable="cmd" dir="${r'$'}{build.drive}/${unit.@bldFile}" failonerror="false">
                            <arg value="/C"/>
                            <arg value="python"/>
                            <arg line ="configure.py --qmake-bin=\epoc32\tools\qmake.bat --qmake-spec=symbian-sbsv2 --platform=symbian --qmake-options=MMP_RULES+=EXPORTUNFROZEN"/>
                        </exec>
                    </then>
                    <else>
                       <echo message="ERROR: Directory ${r'$'}{build.drive}/${unit.@bldFile} doesn't exist."/>
                    </else>
                </if>
          
	  </#if>


            </sequential>
    </#list>
        </parallel>

    </then>
    </if>
    </target>
    
</project>
