<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fn="http://www.w3.org/2005/xpath-functions">

<!-- Initialise keys (sort of like hashes?) to enable us to list distinct packages/severities -->
<xsl:key name="packages" match="stage/step/failures/failure" use="@package"/>
<xsl:key name="severities" match="stage/step/failures" use="@severity"/>

<!-- Main template -->
<xsl:template match="/">
	<html>
	<head><title>Build Status</title></head>
	<body>
	<h1>Build Status</h1>

	<xsl:variable name="criticalCount" select="count(stage/step/failures[@level='critical']/failure)"/>
	<xsl:variable name="majorCount" select="count(stage/step/failures[@level='major']/failure)"/>
	<xsl:variable name="minorCount" select="count(stage/step/failures[@level='minor']/failure)"/>
	<xsl:variable name="unknownCount" select="count(stage/step/failures[@level!='critical' and @level!='major' and @level!='minor']/failure)"/>

	<h2>
	Overall BRAG staus: 
	<xsl:choose>
		<xsl:when test="$criticalCount != 0">BLACK</xsl:when>
		<xsl:when test="$majorCount != 0">RED</xsl:when>
		<xsl:when test="$minorCount != 0">AMBER</xsl:when>
		<xsl:when test="$unknownCount != 0">GREEN</xsl:when>
		<xsl:otherwise>GOLD!</xsl:otherwise>
	</xsl:choose>
	</h2>

	<h2>Breakdown by severity</h2>
	<table border="1">
	<tr><td>Critical</td><td><xsl:value-of select="$criticalCount"/></td></tr>
	<tr><td>Major</td><td><xsl:value-of select="$majorCount"/></td></tr>
	<tr><td>Minor</td><td><xsl:value-of select="$minorCount"/></td></tr>
	<tr><td>Unknown</td><td><xsl:value-of select="$unknownCount"/></td></tr>
	<tr><th>Grand total</th><th><xsl:value-of select="count(stage/step/failures/failure)"/></th></tr>
	</table>

	<h2>Breakdown by stage/step</h2>
	<table border="1">

	<xsl:for-each select="stage">
		<tr>
		<th colspan='2'>Stage: <xsl:value-of select="@name"/></th>
		</tr>
		<xsl:for-each select="step">
			<tr>
			<td colspan='2'>Step: <xsl:value-of select="@name"/></td>
			</tr>
			<xsl:for-each select="failures">
				<tr>
				<td>Failures: <xsl:value-of select="@level"/></td>
				<td>Number: <xsl:value-of select="count(failure)"/></td>
				</tr>
			</xsl:for-each>
		</xsl:for-each>
	</xsl:for-each>

	</table>

	<!-- If any failures are tied to a specific package... -->
	<xsl:if test="stage/step/failures/failure[@package]">
		<h2>Breakdown by package</h2>
		<table border="1">
		<tr><th>Package</th><th>Total failures</th></tr>
		<!-- Use the Muenchian Method to get a set of distinct packages -->
		<xsl:for-each select="stage/step/failures/failure[generate-id(.) = generate-id(key('packages', @package))]">
			<xsl:sort select="@package"/>
			<tr><td><xsl:value-of select="@package"/></td><td><xsl:value-of select="count(key('packages', @package))"/></td></tr>
		</xsl:for-each>
		</table>
	</xsl:if>

	</body>
	</html>
</xsl:template>

</xsl:stylesheet>

