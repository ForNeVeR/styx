param(
	$BuildPath
)

@(
	"$BuildPath\*.dll"
	"$BuildPath\*.pdb"
) | Copy-Item -Destination T:\Temp\miranda\Plugins
