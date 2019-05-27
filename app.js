//jshint esversion:6

const Chart = require('chart.js');
const express = require("express");
const bodyParser =  require("body-parser");
const app = express();


app.use(bodyParser.urlencoded({extended: true}));
app.use(express.static("public"));
app.set('view engine', 'ejs');

app.get("/", function(req, res) {
  var today = new Date();
  var date = today.getDay();

  //res.render('list', {date: date});
  //res.send("Hello");
  res.sendFile(__dirname + "/index.html");
});


app.post("/", function(req, res) {
  var item = req.body.newItme;

});

app.listen(3000, function() {
  console.log("Server started on port 3000");
})
