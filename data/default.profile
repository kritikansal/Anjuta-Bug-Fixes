<?xml version="1.0"?>
<anjuta>
    <plugin name="File Loader"
            url="http://anjuta.org/plugins/"
            mandatory="yes">
		<require group="Anjuta Plugin"
                 attribute="Interfaces"
                 value="IAnjutaFileLoader"/>
    </plugin>
    <plugin name="Starter"
            url="http://anjuta.org/plugins/"
            mandatory="no">
                <require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-starter:StarterPlugin"/>
    </plugin>
    <plugin name="Document Manager"
            url="http://anjuta.org/plugins/"
            mandatory="yes">
		<require group="Anjuta Plugin"
                 attribute="Interfaces"
                 value="IAnjutaDocumentManager"/>
    </plugin>
    <plugin name="File Wizard"
            url="http://anjuta.org/plugins/"
            mandatory="yes">
		<require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-file-wizard:AnjutaFileWizardPlugin"/>
    </plugin>
<!--
    <plugin name="Message Manager"
            url="http://anjuta.org/plugins/"
            mandatory="yes">
		<require group="Anjuta Plugin"
                 attribute="Interfaces"
                 value="IAnjutaMessageManager"/>
    </plugin>
-->
    <plugin name="File Manager"
            url="http://anjuta.org/plugins/"
            mandatory="yes">
		<require group="Anjuta Plugin"
                 attribute="Interfaces"
                 value="IAnjutaFileManager"/>
    </plugin>
    <plugin name="Run Program"
            url="http://anjuta.org/plugins/"
            mandatory="no">
		<require group="Anjuta Plugin"
                 attribute="Location"
                 value="anjuta-run-program:RunProgramPlugin"/>
    </plugin>
</anjuta>
