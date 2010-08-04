<?xml version="1.0"?>
<project name="run-qmake" default="all">

    <target name="all" depends="sf-build-qmake,sf-build-qtextension-tools,sf-configure-orbit,sd-qmake-all-profiles"/>

    <!-- Qmake needs to have been built in this environment, to generate bld.infs to built Qt itself.  
         Qmake.exe only exists in the source tree if its been built  -->
    <target name="sf-build-qmake">
        <if><not><available file="${r'$'}{build.drive}/sf/mw/qt/bin/qmake.exe" type="file"/></not>
            <then>
                <sequential>
  <#list data["//unit[@bldFile = '/sf/mw/qt/src/s60installs/deviceconfiguration']"] as unit>
                    <echo>INFO: Building and configuring qmake for ${unit.@bldFile}</echo>
                    <if>
                        <available file="${r'$'}{build.drive}${unit.@bldFile}" type="dir"/>
                        <then>
                            <exec executable="cmd" dir="${r'$'}{build.drive}${unit.@bldFile}" failonerror="false">
                                <arg value="/C"/>
                                <arg line="sbs -c tools2 -j 4 --logfile=${r'$'}{build.drive}/output/logs/${ant['build.id']}_compile_qmake.log"/>
                            </exec>
                        </then>
                        <else>
                           <echo message="ERROR: Directory ${r'$'}{build.drive}${unit.@bldFile} doesn't exist."/>
                           <fail message="Unit /sf/mw/qt/src/s60installs/deviceconfiguration is in the model, but not present on disk. Cannot build qmake!"/>
                        </else>
                    </if>          
  </#list>
                </sequential>
            </then>
        </if>
    </target>

    <target name="sf-build-qtextension-tools">
        <sequential>
  <#list data["//unit[@bldFile = '/sf/mw/qtextensions/group']"] as unit>
            <echo>INFO: Configuring qtextensions for ${unit.@bldFile}</echo>
            <if>
                <available file="${r'$'}{build.drive}${unit.@bldFile}" type="dir"/>
                <then>
                    <exec executable="cmd" dir="${r'$'}{build.drive}${unit.@bldFile}" failonerror="false">
                        <arg value="/C"/>
                        <arg line="sbs -c tools2 -j 4 --logfile=${r'$'}{build.drive}/output/logs/${ant['build.id']}_compile_qtextensions.log"/>
                    </exec>
                </then>
                <else>
                    <echo message="ERROR: Directory ${r'$'}{build.drive}${unit.@bldFile} doesn't exist."/>
                </else>
            </if>          
  </#list>
        </sequential>
    </target>

    <target name="sf-configure-orbit">
        <sequential>
  <#list data["//unit[@proFile = 'hb.pro']"] as unit>
            <echo>Running configure.py for ${unit.@bldFile}/${unit.@proFile}</echo>
            <if>
                <available file="${r'$'}{build.drive}${unit.@bldFile}" type="dir"/>
                <then>
                    <exec executable="cmd" dir="${r'$'}{build.drive}${unit.@bldFile}" failonerror="false" output="${r'$'}{build.drive}/output/logs/${ant['build.id']}_compile_hb_configure.log">
                        <arg value="/C"/>
                        <arg value="python"/>
                        <arg line ="configure.py --qmake-spec=symbian-sbsv2 --platform=symbian --qmake-options=MMP_RULES+=EXPORTUNFROZEN CONFIG+=sgimagelite_support DEFINES+=HB_EFFECTS_OPENVG DEFINES+=HB_FILTER_EFFECTS"/>
                    </exec>
                        
                    <echo>INFO: Exporting Orbit mkspecs to epoc32\tools for ${unit.@bldFile}/${unit.@proFile}</echo>
                    <echo>INFO: Running Orbit theme installer, make install step</echo>
                    <exec executable="cmd" dir="${r'$'}{build.drive}${unit.@bldFile}" failonerror="false" output="${r'$'}{build.drive}/output/logs/${ant['build.id']}_install_hb_configure.log">
                        <arg value="/C"/>
                        <arg value="make install"/>
                    </exec>
                </then>
                <else>
                    <echo message="ERROR: Directory ${r'$'}{build.drive}${unit.@bldFile} doesn't exist."/>
                </else>
            </if>
  </#list>
        </sequential>
    </target>

    <target name="sd-qmake-all-profiles">
        <parallel threadCount="${r'$'}{number.of.threads}">
  <#list data["//unit[@proFile != 'hb.pro']"] as unit>
            <sequential>
                <echo>Running qmake for ${unit.@bldFile}/${unit.@proFile}</echo>
                <if>
                    <available file="${r'$'}{build.drive}${unit.@bldFile}" type="dir"/>
                    <then>
                        <exec executable="cmd" dir="${r'$'}{build.drive}${unit.@bldFile}" failonerror="false">
                            <arg value="/C"/>
                            <arg value="qmake"/>
                            <!-- arg value="-listgen"/ -->
      <#escape x as x?xml>
                            <arg line="${unit.@qmakeArgs[0]!ant['qt.qmake.default.args']}"/>
      </#escape>
                            <arg value="${unit.@proFile?xml}"/>
                        </exec>
                    </then>
                    <else>
                       <echo message="ERROR: Directory ${r'$'}{build.drive}${unit.@bldFile} doesn't exist."/>
                    </else>
                </if>          
            </sequential>
  </#list>
        </parallel>
    </target>
    
</project>
