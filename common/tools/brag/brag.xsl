<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:fn="http://www.w3.org/2005/xpath-functions"
	>
<xsl:template match="/">
	<html>
	<link href="http://developer.symbian.org/css/reset.css" rel="stylesheet" type="text/css" />
	<link href="http://developer.symbian.org/css/portal-merged.css" rel="stylesheet" type="text/css" />
	<link href="http://developer.symbian.org/css/iefix.css" rel="stylesheet" type="text/css" />
	<head><title>Build Status</title></head>
	<body>
	<h2>Build Status</h2>
	<table border="1">

	<xsl:for-each select="stage">
		<tr>
		<th>Stage: <xsl:value-of select="@name"/></th>
		</tr>
		<xsl:for-each select="step">
			<tr>
			<td>Step: <xsl:value-of select="@name"/></td>
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

	<xsl:variable name="criticalCount" select="count(stage/step/failures[@level='critical']/failure)"/>
	<xsl:variable name="majorCount" select="count(stage/step/failures[@level='major']/failure)"/>
	<xsl:variable name="minorCount" select="count(stage/step/failures[@level='minor']/failure)"/>
	<xsl:variable name="unknownCount" select="count(stage/step/failures[@level!='critical' and @level!='major' and @level!='minor']/failure)"/>

	<p>Total number of critical errors = <xsl:value-of select="$criticalCount"/></p>
	<p>Total number of major errors = <xsl:value-of select="$majorCount"/></p>
	<p>Total number of minor errors = <xsl:value-of select="$minorCount"/></p>
	<p>Total number of unknown errors = <xsl:value-of select="$unknownCount"/></p>

	<h2>
	Final BRAG staus: 
	<xsl:choose>
		<xsl:when test="$criticalCount != 0">BLACK</xsl:when>
		<xsl:when test="$majorCount != 0">RED</xsl:when>
		<xsl:when test="$minorCount != 0">AMBER</xsl:when>
		<xsl:when test="$unknownCount != 0">GREEN</xsl:when>
		<xsl:otherwise>GOLD!</xsl:otherwise>
	</xsl:choose>
	</h2>

	</body>
	</html>
</xsl:template>

</xsl:stylesheet>

