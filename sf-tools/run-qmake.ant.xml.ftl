<?xml version="1.0"?>
<project name="run-qmake" default="all">

    <target name="all" depends="sf-configure-orbit"/>


    <target name="sf-configure-orbit">
        <sequential>
                    <echo>INFO: Configuring Orbit host based tools.</echo>
                    <exec executable="cmd" dir="${r'$'}{build.drive}/sf/mw/hb" failonerror="false" output="${r'$'}{build.drive}/output/logs/${ant['build.id']}_compile_hb_configure.log">
                        <arg value="/C"/>
                        <arg value="python"/>
                        <arg line ="configure.py --host-make-bin=mingw32-make.exe --host-qmake-bin=qmake.exe --qmake-spec=win32-g++ --platform=symbian --qmake-options=MMP_RULES+=EXPORTUNFROZEN CONFIG+=sgimagelite_support DEFINES+=HB_EFFECTS_OPENVG DEFINES+=HB_FILTER_EFFECTS"/>
                    </exec>
                        
                    <echo>INFO: Making host based tools.</echo>
                    <exec executable="cmd" dir="${r'$'}{build.drive}/sf/mw/hb" failonerror="false" output="${r'$'}{build.drive}/output/logs/${ant['build.id']}_compile_hb_make.log">
                        <arg value="/C"/>
                        <arg value="make"/>
                    </exec>
                    <echo>INFO: Exporting Orbit mkspecs to epoc32\tools</echo>
                    <echo>INFO: Running Orbit theme installer, make install step</echo>
                    <exec executable="cmd" dir="${r'$'}{build.drive}/sf/mw/hb" failonerror="false" output="${r'$'}{build.drive}/output/logs/${ant['build.id']}_install_hb_configure.log">
                        <arg value="/C"/>
                        <arg value="make install"/>
                    </exec>
        </sequential>
    </target>

    
</project>
