using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Services;
using System.Data;
using System.Data.SqlClient;
using FirstProject.Data;
using FirstProject.Entities;
using System.Data.Entity;


namespace FirstProject
{
    [WebService(Namespace = "http://tempuri.org/")]
    [WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
    [System.ComponentModel.ToolboxItem(false)]
    // To allow this Web Service to be called from script, using ASP.NET AJAX, uncomment the following line. 
    [System.Web.Script.Services.ScriptService]

    public class WSProject : System.Web.Services.WebService
    {

        [WebMethod]
        public List<User> Read()
        {            
            using (var context = new DataContext("UsersContext"))
            {                
                var users = context.Users.ToList<User>();

                return users;
            }
        }

        [WebMethod]
        public string Delete(int id)
        {
            using (var context = new DataContext("UsersContext"))
            {
                try
                {
                    var user = context.Users.Where(u => u.Id == id).SingleOrDefault();
                    if (user != null)
                    {
                        context.Users.Remove(user);
                        context.SaveChanges();
                        return "User Deleted!";
                    }

                    return "Can't Find that user!";
                }
                catch (Exception ex)
                {
                    return ex.Message;
                }
            }
        }


        [WebMethod]
        public int Create(string firstName, string lastName,
                int age, string gender, bool subscribed)
        {
            using (var context = new DataContext("UsersContext"))
            {
                try
                {
                    var user = new User()
                    {
                        FirstName = firstName,
                        LastName = lastName,
                        Age = age,
                        Gender = gender,
                        Subscribed = subscribed
                    };

                    context.Users.Add(user);
                    context.SaveChanges();

                    return user.Id;
                }
                catch (Exception ex)
                {
                    return -1;
                }
            }
        }


        [WebMethod]
        public string Update(int id, string firstName, string lastName,
                int age, string gender, bool subscribed)
        {
            using (var context = new DataContext("UsersContext"))
            {
                try
                {
                    var updatedUser = new User()
                    {
                        Id = id,
                        FirstName = firstName,
                        LastName = lastName,
                        Age = age,
                        Gender = gender,
                        Subscribed = subscribed
                    };

                    var user = context.Users.Where(u => u.Id == id).SingleOrDefault();
                    if (user != null)
                    {
                        context.Entry(user).CurrentValues.SetValues(updatedUser);
                        context.SaveChanges();
                        return "User Updated!";
                    }

                    return "Can't Find that user!";
                }
                catch (Exception ex)
                {
                    return ex.Message;
                }
            }
        }
    }
}
