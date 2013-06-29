param(
	$BuildPath
)

@(
	"$BuildPath\*.dll"
	"$BuildPath\*.pdb"
) | Copy-Item -Destination D:\Temp\miranda\Plugins
