import express from "express";
import chalk from "chalk";

const app = express();
const PORT = 3000;

app.use(express.urlencoded({ extended: true }));

app.post("/hook", (req, res) => {
  const body: Result = req.body;
  console.log(body.status);
  res.status(200).end();
});

app.get("/hook", (req, res) => {
  res.send("hook endpoint");
});

app.listen(PORT, () =>
  console.log(chalk.greenBright(`🚀 Server running on port ${PORT}`))
);
