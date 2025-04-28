/** @type {import('tailwindcss').Config} */
export default {
    content: [
        "./index.html",
        "./src/**/*.{js,ts,jsx,tsx}",
    ],
    theme: {
        extend: {
            colors: {
                primary: {
                    light: '#4da3ff',
                    DEFAULT: '#0078ff',
                    dark: '#0057b8',
                },
                secondary: {
                    light: '#ff9e7a',
                    DEFAULT: '#ff7846',
                    dark: '#e65b2c',
                },
                background: {
                    light: '#f8f9fa',
                    DEFAULT: '#ffffff',
                    dark: '#121212',
                },
                surface: {
                    light: '#ffffff',
                    DEFAULT: '#f1f5f9',
                    dark: '#1e1e1e',
                },
                text: {
                    light: '#64748b',
                    DEFAULT: '#334155',
                    dark: '#e2e8f0',
                },
            },
            boxShadow: {
                card: '0 4px 6px -1px rgba(0, 0, 0, 0.1), 0 2px 4px -1px rgba(0, 0, 0, 0.06)',
                'card-dark': '0 4px 6px -1px rgba(0, 0, 0, 0.3), 0 2px 4px -1px rgba(0, 0, 0, 0.18)',
            },
            keyframes: {
                'steam-1': {
                    '0%, 100%': { transform: 'translateY(0) scaleY(1)', opacity: '0.7' },
                    '50%': { transform: 'translateY(-10px) scaleY(1.2)', opacity: '0.5' },
                },
                'steam-2': {
                    '0%, 100%': { transform: 'translateY(-5px) scaleY(1.1)', opacity: '0.6' },
                    '60%': { transform: 'translateY(-15px) scaleY(1.3)', opacity: '0.4' },
                },
                'steam-3': {
                    '0%, 100%': { transform: 'translateY(-2px) scaleY(1)', opacity: '0.7' },
                    '70%': { transform: 'translateY(-12px) scaleY(1.2)', opacity: '0.5' },
                },
                'water-flow': {
                    '0%': { transform: 'translateY(-100%)', opacity: '0.8' },
                    '100%': { transform: 'translateY(100%)', opacity: '0' },
                },
                'bounce-in': {
                    '0%': { transform: 'translateY(10px) translateX(-50%)', opacity: '0' },
                    '70%': { transform: 'translateY(-2px) translateX(-50%)', opacity: '1' },
                    '100%': { transform: 'translateY(0) translateX(-50%)', opacity: '1' },
                },
                'fill-cup': {
                    '0%': { height: '0' },
                    '50%': { height: '2px' },
                    '100%': { height: '4px' },
                },
            },
            animation: {
                'steam-1': 'steam-1 3s ease-in-out infinite',
                'steam-2': 'steam-2 3.5s ease-in-out infinite',
                'steam-3': 'steam-3 4s ease-in-out infinite',
                'water-flow': 'water-flow 1.5s linear infinite',
                'bounce-in': 'bounce-in 0.5s ease-out forwards',
                'fill-cup': 'fill-cup 2s ease-in-out forwards',
            },
        },
    },
    plugins: [],
}