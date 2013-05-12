$source = $MyInvocation.MyCommand.Path | Split-Path | Resolve-Path
$javaTarget = "$source/../styx-server/src/main/java"
$cppTarget = "$source/../styx-client/styx-client"

function createPath($path) {
	if (-not (Test-Path $path)) {
		mkdir -Force $path
	}
}

createPath($javaTarget)
createPath($cppTarget)

protoc `
	"-I=$source" `
	--java_out=$javaTarget `
	--cpp_out=$cppTarget `
	$source/Datagram.proto
