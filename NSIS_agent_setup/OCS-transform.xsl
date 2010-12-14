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
  <div align="center">
    <H1> OCS Inventory NG Informations for Computer
      <xsl:value-of select="CONTENT/HARDWARE/NAME"/>
    </H1>
    <hr/>
  </div>              

  <div id="OCSstyle" align="center">

  <h2>General properties</h2>  
  <table>
    <tr>
      <th>NAME</th>
      <th>VALUE</th>
    </tr>
    <xsl:for-each select="CONTENT/HARDWARE/node()">
      <!-- Hide WINDOWS PRODUCT ID and KEY and ELASPED TIME -->
      <xsl:if test="not(name()='WINPRODID') and not(name()='WINPRODKEY') and not(name()='ETIME')">
        <tr>
        <td>
          <xsl:value-of select="name()"/>
        </td>
        <td>
          <xsl:value-of select="."/>
        </td>
        </tr>
      </xsl:if>
    </xsl:for-each>
  </table>

  <h2>BIOS</h2>  
  <table>
    <tr>
      <th>BIOS</th>
      <th>VALUE</th>
    </tr>
    <xsl:for-each select="CONTENT/BIOS/node()">
    <tr>
      <td><xsl:value-of select="name()"/></td>
      <td><xsl:value-of select="."/></td>
    </tr>
    </xsl:for-each>
  </table>

  <h2>Memory slots</h2>  
  <table>
    <xsl:for-each select="/REQUEST/CONTENT/MEMORIES/self::node()">
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
  </table>

  <h2>Expansion slots</h2>  
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
  </table>

  <h2>Port connectors</h2>  
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
  </table>

  <h2>Input devices</h2>  
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
  </table>

  <h2>Controllers</h2>  
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
  </table>

 <h2>Physical storages</h2>  
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
  </table>

  <h2>Logical drives</h2>  
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
  </table>

  <h2>Sound devices</h2>  
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
  </table>

  <h2>Video adapters</h2>  
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
  </table>

  <h2>Monitors</h2>  
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
  </table>

  <h2>Modems</h2>  
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
  </table>

  <h2>Network adapters</h2>  
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
  </table>

  <h2>Printers</h2>  
  <table>
    <xsl:for-each select="/REQUEST/CONTENT/PRINTERS/self::node()">
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
  </table>

  <h2>Software</h2>  
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
  </table>
  </div> 
</body>
</html>

</xsl:template>
</xsl:stylesheet>
