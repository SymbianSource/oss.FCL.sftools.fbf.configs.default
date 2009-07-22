<?xml version="1.0" encoding="utf-8"?>

<diamonds-build>
  <schema>10</schema>
  
<#assign raptor_count=0/>
<#list raptor_summary as raptor_item>
<#assign raptor_count=raptor_count+1 />
</#list>

  <faults>
    <total severity="error">${ant['sf.job.totalyarperrors']}</total>
    <total severity="warning">0</total>
    <total severity="warning_rvct_bad">${raptor_count}</total>
    <total severity="warning_rvct_other">0</total>
    <total severity="codescanner_high">0</total>
    <total severity="codescanner_medium">0</total>
    <total severity="codescanner_low">0</total>
  </faults>
  
  <files>
    <#list files as f>
    <file>
      <type>log</type>
      <name>${f.name}</name>
      <url>file:///${f.path}</url>
    </file>
    </#list>
    <file>
      <type>log</type>
      <name>Raptor summary</name>
      <url>file:///${ant['sf.spec.publish.networkdrive']}\${ant['sf.spec.job.name']}\builds\${ant['sf.spec.job.codeline']}\${ant['build.id']}\logs\raptorbits\summary.csv</url>
    </file>
  </files>
  
</diamonds-build>