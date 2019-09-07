//
//  create_dialog_derived.cpp
//  mac
//
//  Created by Main on 9/6/19.
//  Copyright © 2019 Ravbug. All rights reserved.
//

#include "interface_derived.hpp"

//Declare events here
wxBEGIN_EVENT_TABLE(CreateProjectDialogD, wxDialog)
EVT_BUTTON(wxID_CANCEL,CreateProjectDialogD::OnCancel)
EVT_BUTTON(wxID_OPEN,CreateProjectDialogD::setProjectPath)
EVT_BUTTON(wxID_FILE,CreateProjectDialogD::OnCreate)
wxEND_EVENT_TABLE()

//Construct the dialog
CreateProjectDialogD::CreateProjectDialogD(wxWindow* parent, vector<editor>& versions) : CreateProjectDialog(parent){
	editors = versions;
	//callback = c;
	//Set combo box values to the different Unity versions passed in
	for (editor& e : editors){
		unityVersionChoice->AppendString(e.name);
	}
	//select the element 0
	unityVersionChoice->SetSelection(0);
	
	//populate the template chooser
	loadTemplates(editors[0]);
}

/**
 Called when the Choose button in the dialog is pressed
 Summons an open panel, to allow the user to pick a location to
 write the Unity project
 @param event (unused) command event from wxWidgets
 */
void CreateProjectDialogD::setProjectPath(wxCommandEvent& event){
	string prompt = "Select where to write the Unity project";
	string path = MainFrameDerived::GetPathFromDialog(prompt);
	if (path != ""){
		//set the path
		*projLocTxt << path;
	}
}

/**
 Called when the Create button in the dialog is pressed.
 It checks the form to ensure all fields are filled. If a field is not
 filled correctly, it will show a dialog
 */
void CreateProjectDialogD::OnCreate(wxCommandEvent& event){
	//validate form
	string message = validateForm();
	if (message == ""){
		cout << "form valid" << endl;
		//close and dispose self
		this->EndModal(0);
		delete this;
	}
	else{
		wxMessageBox(message,"Cannot Create Project",wxOK | wxICON_ERROR);
	}
}

/**
 Determines if the dialog has been filled correctly.
 @return empty string if dialog is correct, or a message explaining what in the dialog
 is wrong.
 */
string CreateProjectDialogD::validateForm(){
	//is the name field correct?
	if (projNameTxt->GetValue().length() == 0){
		return "You must specify a name for the project.";
	}
	
	//is the path field correct?
	if (projLocTxt->GetValue().length() == 0){
		return "You must specify a valid location to write the project.";
	}
	
	//is a template selected?
	long itemIndex = -1;
	while ((itemIndex = templateCtrl->GetNextItem(itemIndex,wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED)) != wxNOT_FOUND) {
		break;
	}
	if (itemIndex == -1){
		return "You must select a template.";
	}
	
	return "";
}

/**
 Loads the templates for an editor into the listctrl in the dialog
 @param e the editor struct to load templates for
 */
void CreateProjectDialogD::loadTemplates(editor& e){
	//prepare directory
	//open the folder
	string templatesFolder = e.path + dirsep + e.name + dirsep + templatesDir;
	DIR* dir = opendir(templatesFolder.c_str());
	struct dirent *entry = readdir(dir);
	//loop over the contents
	while (entry != NULL)
	{
		//does the file start with the correct prefix?
		if (string(entry->d_name).rfind(templatePrefix,0)==0){
			//add it to the UI
			wxListItem i;
			i.SetId(0);
			i.SetText(string(entry->d_name).substr(templatePrefix.length()+1));
			
			templateCtrl->InsertItem(i);
		}
		//advance to next item in folder
		entry = readdir(dir);
	}
	//free resources
	closedir(dir);
	free(entry);
}