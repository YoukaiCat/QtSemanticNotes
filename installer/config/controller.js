function Controller()
{
	//add query info
	var queryString = "osGroup=";
	queryString = queryString + installer.value("os");
	queryString = queryString + "&arch=";
	queryString = queryString + systemInfo.currentCpuArchitecture;
	queryString = queryString + "&os=";
	queryString = queryString + systemInfo.productType;
	queryString = queryString + "&kernel=";
	queryString = queryString + systemInfo.kernelType;
	installer.setValue("UrlQueryString", queryString);

	//determine if admin or not
	var isAdmin = false;
	if (installer.value("os") === "win") {
		var testAdmin = installer.execute("cmd", ["/c", "net", "session"]);
		if(testAdmin.length > 1 && testAdmin[1] == 0)
			isAdmin = true;
	} else {
		var testAdmin = installer.execute("id", ["-u"]);
		if(testAdmin.length > 1 && testAdmin[0] == 0)
			isAdmin = true;
	}
	installer.setValue("isAdmin", isAdmin ? "true" : "false");

	//only for installation
	if(installer.isInstaller()) {
		//find the default location, for admin and user for each os
		var targetBase = installer.value("TargetDir");
		if (installer.value("os") === "win") {
			installer.setValue("UserTargetDir", "@HomeDir@/AppData/Local/" + targetBase);
			installer.setValue("AdminTargetDir", "@ApplicationsDir@/" + targetBase);
		} else if(installer.value("os") === "mac") {
			installer.setValue("UserTargetDir", "@HomeDir@/Applications/" + targetBase + ".app");
			installer.setValue("AdminTargetDir", "@ApplicationsDir@/" + targetBase + ".app");
		} else if(installer.value("os") === "x11") {
			installer.setValue("UserTargetDir", "@HomeDir@/" + targetBase);
			installer.setValue("AdminTargetDir", "@ApplicationsDir@/" + targetBase);
		}

		//store all users and online/offline info
		installer.setValue("allUsers", isAdmin ? "true" : "false");
		installer.setValue("isOffline", installer.isOfflineOnly() ? "true" : "false");
	}

	//skip prompt prepare
	if(installer.value("skipPrompt") === "1") {
		installer.statusChanged.connect(function(status) {
			if(status == QInstaller.Success)
				gui.clickButton(buttons.CancelButton);
		});
		installer.updateFinished.connect(function() {
			gui.clickButton(buttons.NextButton);
		});
	}
}

Controller.prototype.IntroductionPageCallback = function()
{
	//Maintenance tool
	if(!installer.isInstaller()) {
		//check if admin neccessarity is given
		if(installer.value("allUsers") === "true" && installer.value("isAdmin") === "false") {//TODO check install admin instead
			QMessageBox.critical("de.skycoder42.advanced-setup.notAdmin",
								 qsTr("Error"),
								 qsTr("The installation was done by an admin/root. Please restart %1 with elevated rights.")
								 .arg(installer.value("MaintenanceToolName")));
			installer.autoAcceptMessageBoxes();
			gui.clickButton(buttons.CancelButton);
			return;
		}

		var widget = gui.currentPageWidget();
		if (widget !== null) {
			if(installer.value("isOffline") === "true") {
				//offline only -> only allow uninstall
				widget.findChild("PackageManagerRadioButton").visible = false;
				widget.findChild("UpdaterRadioButton").visible = false;
				widget.findChild("UninstallerRadioButton").checked = true;
				gui.clickButton(buttons.NextButton);
			} else {
				//Online -> select next step base on the arguments
				if (installer.isUninstaller() && installer.value("uninstallOnly") === "1") {
					widget.findChild("PackageManagerRadioButton").checked = false;
					widget.findChild("UpdaterRadioButton").checked = false;
					widget.findChild("UninstallerRadioButton").checked = true;
					gui.clickButton(buttons.NextButton);
				} else if (installer.isUpdater()) {
					widget.findChild("PackageManagerRadioButton").checked = false;
					widget.findChild("UpdaterRadioButton").checked = true;
					widget.findChild("UninstallerRadioButton").checked = false;
					gui.clickButton(buttons.NextButton);
				} else if(installer.isPackageManager()) {
					widget.findChild("PackageManagerRadioButton").checked = true;
					widget.findChild("UpdaterRadioButton").checked = false;
					widget.findChild("UninstallerRadioButton").checked = false;
					gui.clickButton(buttons.NextButton);
				} //else -> normal start
			}
		}
	}
}

Controller.prototype.DynamicUserPageCallback = function()
{
	// load the admin/user-state for the installation
	var page = gui.pageWidgetByObjectName("DynamicUserPage");
	if(page !== null) {
		if (installer.value("isAdmin") === "true") {
			//admin -> allow both
			page.allUsersButton.checked = installer.value("allUsers") === "true";
			page.meOnlyButton.checked = installer.value("allUsers") !== "true";
			page.allUsersButton.enabled = true;
			page.allUsersLabel.enabled = true;
		} else {
			//user -> allow as user only
			page.meOnlyButton.checked = true;
			page.allUsersButton.checked = false;
			page.allUsersButton.enabled = false;
			page.allUsersLabel.enabled = false;
		}
	}
}

Controller.prototype.TargetDirectoryPageCallback = function()
{
	var page = gui.pageWidgetByObjectName("DynamicUserPage");
	// update the target directory based on the installation scope
	if (page !== null && page.allUsersButton.checked) {
		installer.setValue("allUsers", "true");
		installer.setValue("TargetDir", installer.value("AdminTargetDir"));
	} else {
		installer.setValue("allUsers", "false");
		installer.setValue("TargetDir", installer.value("UserTargetDir"));
	}

	// windows -> if x64 -> modify the path to remove (x86) from it
	if (installer.value("os") === "win" &&
		systemInfo.currentCpuArchitecture.search("64") > 0) {
		var orgFolder = installer.value("TargetDir");
		var programFiles = installer.environmentVariable("ProgramW6432");
		var localProgFiles = installer.environmentVariable("ProgramFiles");
		installer.setValue("TargetDir", orgFolder.replace(localProgFiles, programFiles));
	}

	var widget = gui.currentPageWidget();
	if (widget !== null)// set path with non-native modifiers
		widget.TargetDirectoryLineEdit.text = installer.value("TargetDir").replace("\\", "/");
}

Controller.prototype.ComponentSelectionPageCallback = function() {
	if(installer.value("skipPrompt") === "1") {
		var widget = gui.currentPageWidget();
		widget.selectAll();
		gui.clickButton(buttons.NextButton);
	}
}

Controller.prototype.ReadyForInstallationPageCallback = function() {
	if(installer.value("skipPrompt") === "1")
		gui.clickButton(buttons.NextButton);
}

Controller.prototype.FinishedPageCallback = function()
{
	//windows -> update the registry entry for the installer to work properly
	if (installer.isInstaller() && installer.value("os") === "win") {
		var isAllUsers = (installer.value("allUsers") === "true");
		if(!isAllUsers || installer.gainAdminRights()){
			var args  = [
							installer.value("ProductUUID"),
							"@TargetDir@\\@MaintenanceToolName@.exe",
							isAllUsers ? "HKEY_LOCAL_MACHINE" : "HKEY_CURRENT_USER",
							installer.isOfflineOnly() ? 0 : 1
						];
			installer.execute("@TargetDir@/regSetUninst.bat", args);
		}
	}
}
