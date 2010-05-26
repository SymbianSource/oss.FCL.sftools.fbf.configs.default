<?xml version="1.0"?>
<project name="run-qmake-${ant['sysdef.configuration']}" default="all">

    <target name="all">

    <if><not><available file="${r'$'}{build.drive}/epoc32/tools/qt/qmake.exe" type="file"/></not>
    <then>
	        <sequential>

	<#list data["//unit/@bldFile/.."] as unit>
          <#if (unit.@bldFile=="/sf/mw/qt/src/s60installs/deviceconfiguration")>
                <echo>INFO: Building and configuring qmake for ${unit.@bldFile}</echo>
                <if>
                    <available file="${r'$'}{build.drive}/${unit.@bldFile}" type="dir"/>
                    <then>
                        <exec executable="cmd" dir="${r'$'}{build.drive}/${unit.@bldFile}" failonerror="false">
                            <arg value="/C"/>
                            <arg line="sbs -c tools2 -j 4 --logfile=${r'$'}{build.drive}/output/logs/${ant['build.id']}_compile_qmake.log"/>
                        </exec>
                    </then>
                    <else>
                       <echo message="ERROR: Directory ${r'$'}{build.drive}/${unit.@bldFile} doesn't exist."/>
                       <fail message="Unit /sf/mw/qt/src/s60installs/deviceconfiguration is in the model, but not present on disk. Cannot build qmake!"/>
                    </else>
                </if>          
	      </#if>
    </#list>			
			</sequential>
    </then>
    </if>
	        <sequential>

	<#list data["//unit/@bldFile/.."] as unit>
          <#if (unit.@bldFile=="/sf/mw/qtextensions/group")>
                <echo>INFO: Configuring qtextensions for ${unit.@bldFile}</echo>
                <if>
                    <available file="${r'$'}{build.drive}/${unit.@bldFile}" type="dir"/>
                    <then>
                        <exec executable="cmd" dir="${r'$'}{build.drive}/${unit.@bldFile}" failonerror="false">
                            <arg value="/C"/>
                            <arg line="sbs -c tools2 -j 4 --logfile=${r'$'}{build.drive}/output/logs/${ant['build.id']}_compile_qtextensions.log"/>
                        </exec>
                    </then>
                    <else>
                       <echo message="ERROR: Directory ${r'$'}{build.drive}/${unit.@bldFile} doesn't exist."/>
                    </else>
                </if>          
	      </#if>
    </#list>			
			</sequential>
			
            <sequential>
    <#list data["//unit/@proFile/.."] as unit>
          <#if (unit.@proFile=="hb.pro")>
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
                <echo>INFO: Exporting Orbit mkspecs to epoc32\tools for ${unit.@bldFile}/${unit.@proFile}</echo>

                <copy file="${r'$'}{build.drive}/${unit.@bldFile}/hb_install.prf" todir="${r'$'}{build.drive}/epoc32/tools/qt/mkspecs/features" overwrite="true"/>
                <copy file="${r'$'}{build.drive}/${unit.@bldFile}/hb.prf"         todir="${r'$'}{build.drive}/epoc32/tools/qt/mkspecs/features" overwrite="true"/>
	  </#if>
    </#list>

            </sequential>

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
	  </#if>


            </sequential>
    </#list>
        </parallel>
    </target>
    
</project>
