using FirstProject.Entities;
using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Linq;
using System.Web;

namespace FirstProject.Data
{
    public class DataContext : DbContext
    {
        public DataContext(string stringName) : base(stringName)
        {
        }

        public DbSet<User> Users { get; set; }

    }
}