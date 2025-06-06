import path from "path";
import react from "@vitejs/plugin-react";
import { defineConfig } from "vite";

export default defineConfig({
  plugins: [react()],
  resolve: {
    alias: {
      "@": path.resolve(__dirname, "./src"),
    },
  },
  server: {
    host: process.env.VITE_HOST || '10.15.3.128',
    port: process.env.VITE_PORT || 5173,
    proxy: {
      '/api': {
        target: process.env.VITE_API_URL || 'http://10.15.3.128:8889',
        changeOrigin: true,
        secure: false,
        // rewrite: (path) => path.replace(/^\/api/, ''),
      },
    },
  },
});
