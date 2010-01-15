<?xml version="1.0" encoding="utf-8"?>
<diamonds-build>
  <schema>14</schema>
  <files>
    <file>
      <type>log</type>
      <name>BRAG Summary</name>
      <url>file:///${ant['sf.spec.publish.networkdrive']}\${ant['sf.spec.job.name']}\builds\${ant['sf.spec.job.codeline']}\${ant['build.id']}\summary\_brag.xml</url>
    </file>
    <file>
      <type>log</type>
      <name>Raptor Build Summary</name>
      <url>file:///${ant['sf.spec.publish.networkdrive']}\${ant['sf.spec.job.name']}\builds\${ant['sf.spec.job.codeline']}\${ant['build.id']}\html\index.html</url>
    </file>
    <file>
      <type>log</type>
      <name>Build BOM</name>
      <url>file:///${ant['sf.spec.publish.networkdrive']}\${ant['sf.spec.job.name']}\builds\${ant['sf.spec.job.codeline']}\${ant['build.id']}\build_BOM.zip</url>
    </file>
    <file>
      <type>log</type>
      <name>Build Logs</name>
      <url>file:///${ant['sf.spec.publish.networkdrive']}\${ant['sf.spec.job.name']}\builds\${ant['sf.spec.job.codeline']}\${ant['build.id']}\build_logs.zip</url>
    </file>
    <#if "${ant['sf.spec.smoketest.enable']}" = "true">
      <file>
        <type>log</type>
        <name>Smoke Test Report</name>
        <url>file:///${ant['sf.spec.publish.networkdrive']}\${ant['sf.spec.job.name']}\builds\${ant['sf.spec.job.codeline']}\${ant['build.id']}\ats_reports\ATS3Report.html</url>	  
      </file>
    </#if>
    <#if "${ant['sf.spec.bccheck.enable']}" = "true" && "${ant['sf.spec.bccheck.enable.ha']}" = "true">
      <file>
        <type>log</type>
        <name>Binary Compatibility Report (Headerfiles)</name>
        <url>file:///${ant['sf.spec.publish.networkdrive']}\${ant['sf.spec.job.name']}\builds\${ant['sf.spec.job.codeline']}\${ant['build.id']}\BC\headers_report.xml</url>	  
      </file>
    </#if>
    <#if "${ant['sf.spec.bccheck.enable']}" = "true" && "${ant['sf.spec.bccheck.enable.la']}" = "true">
      <file>
        <type>log</type>
        <name>Binary Compatibility Report (Libraries)</name>
        <url>file:///${ant['sf.spec.publish.networkdrive']}\${ant['sf.spec.job.name']}\builds\${ant['sf.spec.job.codeline']}\${ant['build.id']}\BC\libraries_report.xml</url>	  
      </file>
    </#if>
    <#if "${ant['sf.spec.bccheck.enable.dynamic']}" = "true">
      <file>
        <type>log</type>
        <name>Dynamic BC Test Reports</name>
        <url>file:///${ant['sf.spec.publish.networkdrive']}\${ant['sf.spec.job.name']}\builds\${ant['sf.spec.job.codeline']}\${ant['build.id']}\ats_reports\index.html</url>	  
      </file>
    </#if>
   </files>
</diamonds-build>