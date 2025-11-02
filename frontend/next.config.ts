import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  /* config options here */
  output: "export", // Generate static HTML export
  reactCompiler: true,
  images: {
    unoptimized: true,
  },
};

export default nextConfig;
