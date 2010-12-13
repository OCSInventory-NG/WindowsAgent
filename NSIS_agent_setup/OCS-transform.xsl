<?xml version='1.0' ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">  
    <xsl:template match="/REQUEST">
        <html>
		<head>
<style type="text/css">

#OCSstyle table{
border-collapse:collapse;
width:90%;
}
#OCSstyle th{
	font-family:monospace;
	border:thin solid #b2b7c9;
	padding:0px;
	background-color:#D0E3FA;
	font-size: xx-small;
}
#OCSstyle td{
	font-family:sans-serif;
	border:thin solid #b2b7c9;
	padding:0px;
	text-align:center;
	font-size: xx-small;
}
#OCSstyle caption{
font-family:sans-serif;
}
</style>

		</head>
            <body>
			
                <b>
                    <xsl:value-of select="CONTENT/HARDWARE/NAME"/>
                </b> -- Agent version :
                
                <i>
                    <xsl:value-of select="CONTENT/VERSIONCLIENT"/>
                 </i><hr/>
	<div id="OCSstyle" align="center">
        <table>
          <tr>
            <th>NAME</th>
            <th>VALUE</th>
          </tr>
          <xsl:for-each select="CONTENT/HARDWARE/node()">
          <tr>
            <td><xsl:value-of select="name()"/></td>
            <td><xsl:value-of select="."/></td>
          </tr>
          </xsl:for-each>
        </table><hr/>

        <table>
          <tr>
            <th>BIOS</th>
            <th>BIOS</th>
          </tr>
          <xsl:for-each select="CONTENT/BIOS/node()">
          <tr>
            <td><xsl:value-of select="name()"/></td>
            <td><xsl:value-of select="."/></td>
          </tr>
          </xsl:for-each>
        </table><hr/>
      
                <table>
                <xsl:for-each select="/REQUEST/CONTENT/STORAGES/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>


                <table>
                <xsl:for-each select="CONTENT/DRIVES/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>

                <table>
                <xsl:for-each select="CONTENT/INPUTS/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>
				
                <table >
                <xsl:for-each select="CONTENT/PORTS/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>
				
                <table >
                <xsl:for-each select="CONTENT/SOUNDS/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>
				
                <table >
                <xsl:for-each select="CONTENT/VIDEOS/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>
                <table >
                <xsl:for-each select="CONTENT/CONTROLLERS/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>
                <table >
                <xsl:for-each select="CONTENT/SLOTS/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>
				
                <table >
                <xsl:for-each select="CONTENT/MODEMS/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>
                <table >
                <xsl:for-each select="CONTENT/MONITORS/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>
                <table >
                <xsl:for-each select="CONTENT/NETWORKS/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>
				<table >
                <xsl:for-each select="CONTENT/SOFTWARES/self::node()">
                    <xsl:if test="position()=1" >
                        <tr>
                        <xsl:for-each select="node()">
                            <th><xsl:value-of select="name()"/></th>
                        </xsl:for-each>
                        </tr>
                    </xsl:if>
                    <tr>
                    <xsl:for-each select="node()">
                        <td><xsl:value-of select="."/></td>
                    </xsl:for-each>
                    </tr>
                </xsl:for-each>
                </table><hr/>
	</div> 
				</body>
        </html>
    </xsl:template>



</xsl:stylesheet>
 <!-- Stylus Studio meta-information - (c) 2004-2009. Progress Software Corporation. All rights reserved.

<metaInformation>
	<scenarios>
		<scenario default="yes" name="Scenario1" userelativepaths="yes" externalpreview="no" url="GGD044ST820461-2009-03-16-18-42-18.xml" htmlbaseurl="" outputurl="" processortype="saxon8" useresolver="yes" profilemode="0" profiledepth="" profilelength=""
		          urlprofilexml="" commandline="" additionalpath="" additionalclasspath="" postprocessortype="none" postprocesscommandline="" postprocessadditionalpath="" postprocessgeneratedext="" validateoutput="no" validator="internal"
		          customvalidator="">
			<advancedProp name="sInitialMode" value=""/>
			<advancedProp name="bXsltOneIsOkay" value="true"/>
			<advancedProp name="bSchemaAware" value="true"/>
			<advancedProp name="bXml11" value="false"/>
			<advancedProp name="iValidation" value="0"/>
			<advancedProp name="bExtensions" value="true"/>
			<advancedProp name="iWhitespace" value="0"/>
			<advancedProp name="sInitialTemplate" value=""/>
			<advancedProp name="bTinyTree" value="true"/>
			<advancedProp name="bWarnings" value="true"/>
			<advancedProp name="bUseDTD" value="false"/>
			<advancedProp name="iErrorHandling" value="fatal"/>
		</scenario>
	</scenarios>
	<MapperMetaTag>
		<MapperInfo srcSchemaPathIsRelative="yes" srcSchemaInterpretAsXML="no" destSchemaPath="" destSchemaRoot="" destSchemaPathIsRelative="yes" destSchemaInterpretAsXML="no">
			<SourceSchema srcSchemaPath="GGD044ST820461-2009-03-16-18-42-18.xml" srcSchemaRoot="REQUEST" AssociatedInstance="" loaderFunction="document" loaderFunctionUsesURI="no"/>
		</MapperInfo>
		<MapperBlockPosition>
			<template match="/">
				<block path="html/body/xsl:for-each" x="124" y="17"/>
				<block path="html/body/xsl:for-each/b/xsl:value-of" x="347" y="224"/>
				<block path="html/body/xsl:for-each/xsl:if" x="216" y="197"/>
				<block path="html/body/xsl:for-each/xsl:if/b/xsl:value-of" x="336" y="126"/>
				<block path="html/body/xsl:for-each/xsl:if/xsl:value-of" x="330" y="11"/>
			</template>
			<template match="/REQUEST">
				<block path="html/body/b/xsl:value-of" x="271" y="108"/>
				<block path="html/body/i/xsl:value-of" x="271" y="72"/>
				<block path="html/body/xsl:for-each" x="321" y="6"/>
				<block path="html/body/xsl:for-each/xsl:value-of" x="271" y="36"/>
				<block path="html/body/xsl:for-each/xsl:value-of[1]" x="231" y="36"/>
				<block path="html/body/xsl:for-each/xsl:value-of[2]" x="191" y="36"/>
			</template>
			<template name="tes">
				<block path="xsl:for-each" x="242" y="70"/>
			</template>
			<template match="auteur">
				<block path="h2/xsl:value-of" x="336" y="54"/>
			</template>
			<template match="/REQUEST/CONTENT/MEMORIES"></template>
		</MapperBlockPosition>
		<TemplateContext></TemplateContext>
		<MapperFilter side="target">
			<Fragment url="" path="" action=""/>
		</MapperFilter>
	</MapperMetaTag>
</metaInformation>
-->