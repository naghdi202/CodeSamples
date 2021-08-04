var dataArray = [];
var Genders = [
    { genderId: 0, name: "Male" },
    { genderId: 1, name: "Female" },
    { genderId: 2, name: "Other" },
];

var record = 0;

var nextDataID;

function getIndexByID(id) {
    var idx,
        l = dataArray.length;

    for (var j = 0; j < l; j++) {
        if (dataArray[j].Id == id) {
            return j;
        }
    }
    return null;
}



$(document).ready(function () {
    AjaxRead();

    var gridDataSource = new kendo.data.DataSource({
        transport: {
            read: function (e) {
                e.success(dataArray);
            },
            
            update: function (e) {
                updateAjax(e.data);
                e.success();
            },

            destroy: function (e) {
                deleteAjax(e.data.Id);
                e.success();           
            },

            create: function(e) {
                submitAjax(e.data);
                e.success();
            }
        },

        error: function (e) {
            alert("Status: " + e.status + "; Error message: " + e.errorThrown);
        },

        schema: {
            model: {
                id: "Id",
                fields: {
                    Id: { editable: false },
                    FirstName: { type: "string", validation: { required: true } },
                    LastName: { type: "string" },
                    Age: { type: "number", validation: { required: true, min: 1 } },
                    Gender: { type: "string" },
                    Subscribed: { type: "boolean" },
                }
            }
        },

        pageSize: 15,
        batch: false

    });

    $("#grid").kendoGrid({
        dataSource: gridDataSource,
        pageable: true,
        editable: "inline",
        sortable: true,
        toolbar: ["create"],
        columns: [{
            title: "Row No.",
            template: "#= ++record #",
            editable: false
        },
        {
            field: "FirstName",
            title: "First Name"
        }, {
            field: "LastName",
            title: "Last Name"
        }, {
            field: "Gender",
            title: "Gender",
            editor: genderDropDownEditor
        }, {
            field: "Age",
        }, {
            field: "Subscribed",
            template: '<input type="checkbox" #= Subscribed ? \'checked="checked"\' : "" # disabled class="chkbx" />',
            editor: customBoolEditor
        }, {
            command: ["edit", "destroy"], title: "&nbsp;",
        }],

        dataBinding: function () {
            record = (this.dataSource.page() - 1) * this.dataSource.pageSize();
        }
    });

    function customBoolEditor(container, options) {
        $('<input class="k-checkbox" type="checkbox" name="Subscribed" data-type="boolean" data-bind="checked:Subscribed">').appendTo(container);

        return;
    };

    function genderDropDownEditor(container) {
        var input = $('<input id="genderId" name="Gender">');
        input.appendTo(container);

        input.kendoDropDownList({
            dataTextField: "name",
            dataValueField: "name",
            dataSource: Genders
        }).appendTo(container);

        return;
    };
})

function AjaxRead() {
    $.ajax({
        type: "POST",
        async: false,
        url: "https://localhost:44324/WSProject.asmx/Read",
        contentType: "application/json; charset=utf-8",
        dataType: "json",
        success: succes_AjaxRead,
    });
}

function succes_AjaxRead(respond) {
    dataArray = respond.d;
}

function submitButton() {
    nextDataID = dataArray.length + 1;
    var inputData = {
        firstName: $("#firstName").val(),
        lastName: $("#lastName").val(),
        age: Number($("#age").val()),
        gender: $("#gender").val(),
        subscribed: $("#subscribe").prop('checked')
    };
    
    submitAjax(JSON.stringify(inputData));
}

function submitAjax(inputData) {
    debugger;
    $.ajax({
        type: "POST",
        async: false,
        url: "https://localhost:44324/WSProject.asmx/Create",
        contentType: "application/json; charset=utf-8",
        data: inputData,
        dataType: "json",        
        success: function(e) {
            var inpData = JSON.parse(inputData);
            console.log(e.d);
            var submitedRow = {
                Id: e.d,
                FirstName: inpData.firstName,
                LastName: inpData.lastName,
                Age: inpData.age,
                Gender: inpData.gender,
                Subscribed: inpData.subscribed
            } 
            dataArray.push(submitedRow);

            var grid = $("#grid").data("kendoGrid");
            grid.dataSource.read();
            grid.refresh();
        }, 
        error:console.log("error"),
    });
}

function deleteAjax(id) {
    $.ajax({
        type: "POST",
        async: false,
        url: "https://localhost:44324/WSProject.asmx/Delete",
        data: {Id: id},
        dataType: "json",
        success: dataArray.splice(getIndexByID(id), 1),
    });
}

function updateAjax(inputData) {
    $.ajax({
        type: "POST",
        async: false,
        url: "https://localhost:44324/WSProject.asmx/Update",
        data: inputData,
        dataType: "json",
        success: dataArray[getIndexByID(inputData.Id)] = inputData,
    });
}
