<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:include href="include2/level2_include1.xsl"/>

<xsl:template match="one-tag">
  inside level1_include2.xsl, including level2_include1.xsl
  <xsl:apply-templates/>
</xsl:template>

</xsl:stylesheet>