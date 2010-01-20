<?xml version="1.0"?>
 <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
 	<xsl:output method="xml" indent="yes"/>
<!-- create a stand-alone sysdef from a linked set of fragments -->

<xsl:param name="path">/os/deviceplatformrelease/foundation_system/system_model/system_definition.xml</xsl:param>

<xsl:template match="/*">
	<xsl:apply-templates select="." mode="join"/>
</xsl:template>


<xsl:template match="/SystemDefinition[systemModel]">
<xsl:apply-templates select="." mode="join">
	<xsl:with-param name="filename" select="$path"/>
</xsl:apply-templates>
</xsl:template>




<xsl:include href="joinsysdef-module.xsl"/>

</xsl:stylesheet>
