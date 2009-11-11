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

	<xsl:variable name="criticalCount" select="count(phase/step/failures[@level='critical']/failure)"/>
	<xsl:variable name="majorCount" select="count(phase/step/failures[@level='major']/failure)"/>
	<xsl:variable name="minorCount" select="count(phase/step/failures[@level='minor']/failure)"/>
	<xsl:variable name="unknownCount" select="count(phase/step/failures[@level!='critical' and @level!='major' and @level!='minor']/failure)"/>

	<h2>
	Overall BRAGG staus: 
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
	<tr><th>Grand total</th><th><xsl:value-of select="count(phase/step/failures/failure)"/></th></tr>
	</table>

	<h2>Breakdown by phase/step</h2>
	<table border="1">

	<xsl:for-each select="phase">
		<tr>
		<th colspan='2'>Phase: <xsl:value-of select="@name"/></th>
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
	<xsl:if test="phase/step/failures/failure[@package]">
		<h2>Breakdown by package</h2>
		<table border="1">
		<tr><th>Package</th><th>Total failures</th></tr>
		<!-- Use the Muenchian Method to get a set of distinct packages -->
		<xsl:for-each select="phase/step/failures/failure[generate-id(.) = generate-id(key('packages', @package))]">
			<xsl:sort select="@package"/>
			<tr>
			<td><a><xsl:attribute name="href"><xsl:value-of select="concat('#package', @package)"/></xsl:attribute><xsl:value-of select="@package"/></a></td>
			<td><xsl:value-of select="count(key('packages', @package))"/></td>
			</tr>
		</xsl:for-each>
		</table>

		<h2>Breakdown by package/severity</h2>
		<xsl:for-each select="phase/step/failures[@level]/failure[generate-id(.) = generate-id(key('packages', @package))]">
			<xsl:sort select="@package"/>
			<xsl:variable name="package" select="@package"/>
			<a><xsl:attribute name="name"><xsl:value-of select="concat('package', $package)"/></xsl:attribute>
			<h3><xsl:value-of select="$package"/></h3>
			</a>
			<table border="1">
			<tr><th>Severity</th><th>Count</th></tr>
			<xsl:for-each select="/buildStatus/phase/step/failures[generate-id(.) = generate-id(key('severities', @level))]">
				<xsl:variable name="severity" select="@level"/>
				<tr>
				<td><a>
				<xsl:if test="count(/buildStatus/phase/step/failures[@level = $severity]/failure[@package = $package]) != 0">
					<xsl:attribute name="href"><xsl:value-of select="concat('#', $severity, $package)"/></xsl:attribute>
				</xsl:if>
				<xsl:value-of select="$severity"/></a></td>
				<td><xsl:value-of select="count(key('packageANDseverity', concat($severity, $package)))"/></td>
				</tr>
			</xsl:for-each>
			</table>
		</xsl:for-each>
	</xsl:if>

	<h2>Floating failures by phase/step/severity</h2>
	<xsl:if test="count(phase/step/failures/failure/@package) = count(phase/step/failures/failure)">
		<p>No errors independent of package</p>
	</xsl:if>
	<xsl:for-each select="phase[count(step/failures/failure/@package) != count(step/failures/failure)]">
		<h3>Phase: <xsl:value-of select="@name"/></h3>
		<xsl:for-each select="step[count(failures/failure/@package) != count(failures/failure)]">
			<h4>Step: <xsl:value-of select="@name"/></h4>
			<xsl:for-each select="failures[count(failure/@package) != count(failure)]">
				<dl><dt><xsl:value-of select="@level"/></dt><dd>
				<ul>
				<xsl:for-each select="failure[count(@package) = 0]">
					<li><xsl:value-of select="."/></li>
				</xsl:for-each>
				</ul>
				</dd></dl>
			</xsl:for-each>
		</xsl:for-each>
	</xsl:for-each>

	<h2>Package failures by package/severity</h2>
	<xsl:if test="count(phase/step/failures/failure/@package) = 0">
		<p>No errors specific to a package</p>
	</xsl:if>
	<xsl:for-each select="phase/step/failures[@level]/failure[generate-id(.) = generate-id(key('packages', @package))]">
		<xsl:sort select="@package"/>
		<xsl:variable name="package" select="@package"/>
		<a><xsl:attribute name="name"><xsl:value-of select="concat('package', $package)"/></xsl:attribute>
		<h3><xsl:value-of select="$package"/></h3>
		</a>
		<xsl:for-each select="/buildStatus/phase/step/failures[generate-id(.) = generate-id(key('severities', @level))]">
			<xsl:variable name="severity" select="@level"/>
			<xsl:if test="count(/buildStatus/phase/step/failures[@level = $severity]/failure[@package = $package]) != 0">
				<dl><dt><a><xsl:attribute name="name"><xsl:value-of select="concat($severity, $package)"/></xsl:attribute>
					<xsl:value-of select="$severity"/> (<xsl:value-of select="count(/buildStatus/phase/step/failures[@level = $severity]/failure[@package = $package])"/>)
				</a></dt><dd>
				<ul>
				<xsl:for-each select="/buildStatus/phase/step/failures[@level = $severity]/failure[@package = $package]">
					<xsl:sort select="@package"/>
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
			</xsl:if>
		</xsl:for-each>
	</xsl:for-each>

	</body>
	</html>
</xsl:template>

</xsl:stylesheet>

