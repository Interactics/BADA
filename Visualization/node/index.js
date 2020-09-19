var path = require('path');
var bodyParser = require('body-parser');
var express = require('express');

var app = express();
app.use(bodyParser.json());

var staticPath = path.join(__dirname, '/public');

var indexRouter = require('./routes/index');

app.engine('html', require('ejs').renderFile);
app.set('view engine', 'html');

app.use('/static', express.static(staticPath));

app.use('/', indexRouter);

app.listen(3000, function() {
  console.log('listening');
});
