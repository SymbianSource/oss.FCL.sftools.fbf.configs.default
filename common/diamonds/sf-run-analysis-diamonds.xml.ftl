<?xml version="1.0" encoding="utf-8"?>

<diamonds-build>
  <schema>14</schema>
  
<#assign raptor_errors=0/>
<#assign raptor_warnings=0/>
<#list raptor_summary as raptor_item>
<#if raptor_item.category = "raptor_error">
  <#assign raptor_errors=raptor_errors+1 />
<#elseif raptor_item.category = "raptor_warning">
  <#assign raptor_warnings=raptor_warnings+1 />
</#if>
</#list>

  <faults>
    <total severity="Raptor Errors">${raptor_errors}</total>
    <total severity="Raptor Warnings">${raptor_warnings}</total>
    <total severity="Recipes Failed">${ant['sf.job.totalyarperrors']}</total>
  </faults>
</diamonds-build>