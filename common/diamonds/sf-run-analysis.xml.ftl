<?xml version="1.0" encoding="utf-8"?>
<diamonds-build>
  <schema>10</schema>

  <faults>
    <total severity="error">${ant['sf.job.totalyarperrors']}</total>
    <total severity="warning">0</total>
    <total severity="warning_rvct_bad">0</total>
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
  </files>
  
</diamonds-build>