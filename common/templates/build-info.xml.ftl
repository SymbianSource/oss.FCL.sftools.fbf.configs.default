<?xml version="1.0" encoding="utf-8"?>

<#assign row = 0 />

<diamonds-build>
  <content>
    <project>
      <name>Config</name>
      <#list config as c>
 	  <baseline>${c.loc}#${c.rev}</baseline>
 	  </#list>
    </project>
    <project>
      <name>Project</name>
      <#list project as p>
 	  <baseline>${p.loc}#${p.rev}</baseline>
 	  </#list>
    </project>
    <project>
      <name>Baseline</name>
      <#list baseline as b>
      <baseline>${b}</baseline>
      </#list>
    </project>
    <project>
      <name>Tools Baseline</name>
      <#list toolsbaseline as b>
      <baseline>${b}</baseline>
      </#list>
    </project>
    <project>
 	  <name>Sources</name>
 	  <#list sources as s>
	  <#if (row!=0)>
 	  <baseline>${s.loc}#${s.rev}</baseline>
	  </#if>
	  <#assign row = row + 1 />
 	  </#list>
    </project>
  </content>
</diamonds-build>
