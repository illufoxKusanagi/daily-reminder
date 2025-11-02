import { DataTable } from "@/components/charts/table";
import { ModeToggle } from "@/components/mode-toggle";
import { Calendar } from "@/modules/calendar/calendar";

export default function Home() {
  return (
    <div className="flex flex-col min-h-screen mx-32 items-center justify-center font-sans dark:bg-black">
      <div className="absolute top-4 right-4">
        <ModeToggle />
      </div>
      <h2>Daily Reminder</h2>
      <p>
        Your app to boost NNN (Nonstop Ngoding November), so you won&apos;t
        scroll fesnuk. Be productive and happy vibe coding!
      </p>
      {/* <DataTable /> */}
      <Calendar />
    </div>
  );
}
