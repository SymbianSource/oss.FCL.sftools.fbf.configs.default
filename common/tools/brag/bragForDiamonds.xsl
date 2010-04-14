<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!-- Main template -->
<xsl:template match="/buildStatus">
	<xsl:variable name="criticalCount" select="count(phase/step/failures[@level='critical']/failure)+sum(phase/step/failures[@level='critical']/@count)"/>
	<xsl:variable name="majorCount" select="count(phase/step/failures[@level='major']/failure)+sum(phase/step/failures[@level='major']/@count)"/>
	<xsl:variable name="minorCount" select="count(phase/step/failures[@level='minor']/failure)+sum(phase/step/failures[@level='minor']/@count)"/>
	<xsl:variable name="unknownCount" select="count(phase/step/failures[@level!='critical' and @level!='major' and @level!='minor']/failure)+sum(phase/step/failures[@level!='critical' and @level!='major' and @level!='minor']/@count)"/>

	<diamonds-build>
	<schema>13</schema>
	<build>
	<status>
	<xsl:choose>
		<xsl:when test="$criticalCount != 0">Red</xsl:when>
		<xsl:when test="$majorCount != 0">Amber</xsl:when>
		<xsl:when test="$unknownCount != 0">Amber</xsl:when>
		<xsl:otherwise>Green</xsl:otherwise>
	</xsl:choose>
	</status>
	</build>
	</diamonds-build>
</xsl:template>

</xsl:stylesheet>

