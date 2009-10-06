<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!-- Main template -->
<xsl:template match="/buildStatus">
	<xsl:variable name="criticalCount" select="count(phase/step/failures[@level='critical']/failure)"/>
	<xsl:variable name="majorCount" select="count(phase/step/failures[@level='major']/failure)"/>
	<xsl:variable name="minorCount" select="count(phase/step/failures[@level='minor']/failure)"/>
	<xsl:variable name="unknownCount" select="count(phase/step/failures[@level!='critical' and @level!='major' and @level!='minor']/failure)"/>

	<diamonds-build>
	<schema>13</schema>
	<build>
	<status>
	<xsl:choose>
		<xsl:when test="$criticalCount != 0">Black</xsl:when>
		<xsl:when test="$majorCount != 0">Red</xsl:when>
		<xsl:when test="$minorCount != 0">Amber</xsl:when>
		<xsl:when test="$unknownCount != 0">Green</xsl:when>
		<xsl:otherwise>Gold</xsl:otherwise>
	</xsl:choose>
	</status>
	</build>
	</diamonds-build>
</xsl:template>

</xsl:stylesheet>
