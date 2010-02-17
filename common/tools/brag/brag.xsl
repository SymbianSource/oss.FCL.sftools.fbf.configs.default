<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fn="http://www.w3.org/2005/xpath-functions">

<!-- Initialise keys (sort of like hashes?) to enable us to list distinct packages/severities -->
<xsl:key name="packages" match="failure" use="@package"/>
<xsl:key name="severities" match="/buildStatus/phase/step/failures" use="@level"/>
<xsl:key name="packageANDseverity" match="/buildStatus/phase/step/failures[@level]/failure[@package]" use="concat(../@level, @package)"/>

<!-- Main template -->
<xsl:template match="/buildStatus">
	<html>
	<head><title>Build Status</title></head>
	<body>
	<h1>Build Status</h1>

	<xsl:variable name="criticalCount" select="count(phase/step/failures[@level='critical']/failure)+sum(phase/step/failures[@level='critical']/@count)"/>
	<xsl:variable name="majorCount" select="count(phase/step/failures[@level='major']/failure)+sum(phase/step/failures[@level='major']/@count)"/>
	<xsl:variable name="minorCount" select="count(phase/step/failures[@level='minor']/failure)+sum(phase/step/failures[@level='minor']/@count)"/>
	<xsl:variable name="unknownCount" select="count(phase/step/failures[@level!='critical' and @level!='major' and @level!='minor']/failure)+sum(phase/step/failures[@level!='critical' and @level!='major' and @level!='minor']/@count)"/>

	<h2>
	Overall BRAGG staus: 
	<xsl:choose>
		<xsl:when test="$criticalCount != 0">BLACK</xsl:when>
		<xsl:when test="$majorCount != 0">RED</xsl:when>
		<xsl:when test="$unknownCount != 0">RED</xsl:when>
		<xsl:when test="$minorCount != 0">AMBER</xsl:when>
		<xsl:otherwise>GREEN</xsl:otherwise>
	</xsl:choose>
	</h2>

	<h2>Breakdown by severity</h2>
	<table border="1">
	<tr><td>Critical</td><td><xsl:value-of select="$criticalCount"/></td></tr>
	<tr><td>Major</td><td><xsl:value-of select="$majorCount"/></td></tr>
	<tr><td>Minor</td><td><xsl:value-of select="$minorCount"/></td></tr>
	<tr><td>Unknown</td><td><xsl:value-of select="$unknownCount"/></td></tr>
	<tr><th>Grand total</th><th><xsl:value-of select="$criticalCount+$majorCount+$minorCount+$unknownCount"/></th></tr>
	</table>

	<h2>Breakdown by phase/step</h2>
	<table border="1">

	<xsl:for-each select="phase">
		<tr>
		<th colspan='2'>Phase: <xsl:value-of select="@name"/></th>
		</tr>
		<xsl:for-each select="step">
			<tr>
			<xsl:choose>
			<xsl:when test="@detailshref">
			<td colspan='2'>Step: <a><xsl:attribute name="href"><xsl:value-of select="@detailshref"/></xsl:attribute><xsl:value-of select="@name"/></a></td>
			</xsl:when>
			<xsl:otherwise>
			<td colspan='2'>Step: <xsl:value-of select="@name"/></td>
			</xsl:otherwise>
			</xsl:choose>
			</tr>
			<xsl:for-each select="failures">
				<tr>
				<td>Failures: <xsl:value-of select="@level"/></td>
				
				<xsl:choose>
				<xsl:when test="@count">
				<td>Number: <xsl:value-of select="@count"/></td>
				</xsl:when>
				<xsl:otherwise>
				<td>Number: <xsl:value-of select="count(failure)"/></td>
				</xsl:otherwise>
				</xsl:choose>
				</tr>
			</xsl:for-each>
		</xsl:for-each>
	</xsl:for-each>

	</table>

	<h2>Floating failures by phase/step/severity</h2>
	<xsl:if test="count(phase/step/failures/failure/@package) = count(phase/step/failures/failure)">
		<p>No failures to show. Please also check the <a href="../html/index.html">Raptor build summary</a> for details on that part of the build</p>
	</xsl:if>
	<xsl:for-each select="phase[count(step/failures/failure/@package) != count(step/failures/failure)]">
		<h3>Phase: <xsl:value-of select="@name"/></h3>
		<xsl:for-each select="step[count(failures/failure/@package) != count(failures/failure)]">
			<h4>Step: <xsl:value-of select="@name"/></h4>
			<xsl:for-each select="failures[count(failure/@package) != count(failure)]">
				<xsl:variable name="severity" select="@level"/>
				<dl><dt><xsl:value-of select="@level"/> (<xsl:value-of select="count(/buildStatus/phase/step/failures[@level = $severity]/failure[count(@package) = 0])"/>)</dt><dd>
				<ul>
				<xsl:for-each select="failure[count(@package) = 0]">
					<li><xsl:value-of select="effect"/></li>
					<xsl:if test="@unreported_causes != '0'">
						<br/>(Too much text to show everything; <xsl:value-of select="@unreported_causes"/> lines not shown.)
					</xsl:if>
					<xsl:for-each select="causes">
						<pre><xsl:value-of select="."/></pre>
					</xsl:for-each>
				</xsl:for-each>
				</ul>
				</dd></dl>
			</xsl:for-each>
		</xsl:for-each>
	</xsl:for-each>

	</body>
	</html>
</xsl:template>

</xsl:stylesheet>

