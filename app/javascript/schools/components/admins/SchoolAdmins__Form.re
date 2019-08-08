[@bs.config {jsx: 3}];

let str = React.string;

module CreateSchoolAdminQuery = [%graphql
  {|
  mutation($name: String!, $email: String!) {
    createSchoolAdmin(name: $name, email: $email){
      schoolAdmin{
        id,
        avatarUrl
      }
    }
  }
|}
];

module UpdateSchoolAdminQuery = [%graphql
  {|
  mutation($id: ID!, $name: String!, $email: String!) {
    updateSchoolAdmin(id: $id, name: $name, email: $email) {
      success
    }
  }
|}
];

let createSchoolAdminQuery =
    (authenticityToken, email, name, setSaving, updateCB) => {
  setSaving(_ => true);
  CreateSchoolAdminQuery.make(~email, ~name, ())
  |> GraphqlQuery.sendQuery(authenticityToken)
  |> Js.Promise.then_(response => {
       switch (response##createSchoolAdmin##schoolAdmin) {
       | Some(schoolAdmin) =>
         updateCB(
           SchoolAdmin.create(
             schoolAdmin##id,
             name,
             email,
             schoolAdmin##avatarUrl,
           ),
         );
         Notification.success(
           "Success",
           "School Admin created successfully.",
         );
       | None => setSaving(_ => false)
       };
       Js.Promise.resolve();
     })
  |> ignore;
};

let updateSchoolAdminQuery =
    (authenticityToken, admin, email, name, setSaving, updateCB) => {
  setSaving(_ => true);
  let id = admin |> SchoolAdmin.id;
  UpdateSchoolAdminQuery.make(~id, ~email, ~name, ())
  |> GraphqlQuery.sendQuery(authenticityToken)
  |> Js.Promise.then_(response => {
       response##updateSchoolAdmin##success ?
         {
           updateCB(
             SchoolAdmin.create(
               id,
               name,
               email,
               admin |> SchoolAdmin.avatarUrl,
             ),
           );
           Notification.success(
             "Success",
             "School Admin updated successfully.",
           );
         } :
         setSaving(_ => false);
       Js.Promise.resolve();
     })
  |> ignore;
};

let handleButtonClick =
    (authenticityToken, admin, setSaving, name, email, updateCB, event) => {
  event |> ReactEvent.Mouse.preventDefault;
  switch (admin) {
  | Some(admin) =>
    updateSchoolAdminQuery(
      authenticityToken,
      admin,
      email,
      name,
      setSaving,
      updateCB,
    )
  | None =>
    createSchoolAdminQuery(
      authenticityToken,
      email,
      name,
      setSaving,
      updateCB,
    )
  };
};
let isInvalidEmail = email =>
  email |> EmailUtils.isInvalid(~allowBlank=false);
let saveDisabled = (email, name, saving, admin) => {
  let dirty =
    switch (admin) {
    | Some(admin) =>
      admin |> SchoolAdmin.name == name && admin |> SchoolAdmin.email == email
    | None => false
    };

  isInvalidEmail(email) || saving || name == "" || dirty;
};

let buttonText = (email, name, saving, admin) =>
  switch (saving, email == "", isInvalidEmail(email), name == "") {
  | (true, false | true, false | true, false | true) => "Saving"
  | (false, true, false | true, false | true) => "Enter Email"
  | (false, false, true, false | true) => "Enter a valid Email"
  | (false, false, false, true) => "Enter full name"
  | (false, false, false, false) =>
    switch (admin) {
    | Some(_) => "Update School Admin"
    | None => "Create School Admin"
    }
  };

[@react.component]
let make = (~authenticityToken, ~admin, ~updateCB) => {
  let (saving, setSaving) = React.useState(() => false);
  let (name, setName) =
    React.useState(() =>
      switch (admin) {
      | Some(admin) => admin |> SchoolAdmin.name
      | None => ""
      }
    );
  let (email, setEmail) =
    React.useState(() =>
      switch (admin) {
      | Some(admin) => admin |> SchoolAdmin.email
      | None => ""
      }
    );
  <div className="w-full">
    <DisablingCover disabled=saving>
      <div className="mx-auto bg-white">
        <div
          className="flex items-centre font-bold py-6 pl-16 mb-4 bg-gray-200">
          {
            (
              switch (admin) {
              | Some(_) => "Update school admin"
              | None => "Add new school admin"
              }
            )
            |> str
          }
        </div>
        <div className="max-w-2xl p-6 mx-auto">
          <div>
            <label
              className="inline-block tracking-wide text-xs font-semibold mb-2"
              htmlFor="email">
              {"Email" |> str}
            </label>
            <span> {"*" |> str} </span>
            <input
              value=email
              onChange={
                event => setEmail(ReactEvent.Form.target(event)##value)
              }
              className="appearance-none block w-full bg-white border border-gray-400 rounded py-3 px-4 leading-tight focus:outline-none focus:bg-white focus:border-gray-500"
              id="email"
              type_="email"
              placeholder="Add email here"
            />
          </div>
          <div className="mt-5">
            <label
              className="inline-block tracking-wide text-xs font-semibold mb-2"
              htmlFor="name">
              {"Name" |> str}
            </label>
            <span> {"*" |> str} </span>
            <input
              value=name
              onChange={
                event => setName(ReactEvent.Form.target(event)##value)
              }
              className="appearance-none block w-full bg-white border border-gray-400 rounded py-3 px-4 leading-tight focus:outline-none focus:bg-white focus:border-gray-500"
              id="name"
              type_="text"
              placeholder="add name here"
            />
          </div>
          <div className="w-auto mt-8">
            <button
              disabled={saveDisabled(email, name, saving, admin)}
              onClick={
                handleButtonClick(
                  authenticityToken,
                  admin,
                  setSaving,
                  name,
                  email,
                  updateCB,
                )
              }
              className="w-full btn btn-large btn-primary">
              {buttonText(email, name, saving, admin) |> str}
            </button>
          </div>
        </div>
      </div>
    </DisablingCover>
  </div>;
};
