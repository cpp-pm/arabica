<?xml version="1.0"?> 
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:my="http://my.com">

<!-- XSLT - Numbering - AVT attributes -->
<!-- Created : Khalil Jabrane -->
<!-- Date: 04/18/2000 -->

	<xsl:output method="xml" encoding="UTF-16"/>

	<xsl:template match="/">
		<xsl:variable name="grouping-size" select="'2'"/>
        <xsl:variable name="grouping-separator" select="','"/>
        <xsl:variable name="letter-val" select="'trad'"/>
        <xsl:variable name="format" select="'&#x05d0;'"/>
        <xsl:variable name="lang" select="'k'"/>

        <!-- Test AVT attributes (grouping-size, grouping-separator, format, letter-value, format) -->
        <xsl:number value="99999" grouping-size="{$grouping-size + 1}" grouping-separator="{concat($grouping-separator, '.')}"/><BR/>
        <xsl:number value="3999" format="{$format}" letter-value="{concat($letter-val, 'itional')}"/><BR/>
        <xsl:number value="99999" format="{''}"/><BR/>
        <xsl:number value="1000" format="{'&#x4e01;'}" lang="{concat($lang, 'o')}"/><BR/>
	</xsl:template>
   
</xsl:stylesheet>  
