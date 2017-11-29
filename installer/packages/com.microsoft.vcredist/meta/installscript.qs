function Component(){}

Component.prototype.createOperations = function()
{
	try {
		component.createOperations();
		if (installer.value("os") === "win") {
			component.addElevatedOperation("Execute", "@TargetDir@/vcredist_x64.exe", "/norestart");
		}
	} catch (e) {
		print(e);
	}
}
