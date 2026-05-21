import app from './app';

const PORT = parseInt(process.env.PORT || '4010', 10);

app.listen(PORT, () => {
  console.log(`Pokemon MUD server started: http://127.0.0.1:${PORT}`);
  console.log(`API health check: http://127.0.0.1:${PORT}/api/health`);
});
