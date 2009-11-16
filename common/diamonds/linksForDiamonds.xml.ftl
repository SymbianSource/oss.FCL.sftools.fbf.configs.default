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
   </files>
</diamonds-build>