import { DataTable } from "@/components/charts/table";
import { ModeToggle } from "@/components/mode-toggle";
import { Calendar } from "@/modules/calendar/calendar";

export default function Home() {
  return (
    <div className="flex flex-col h-screen p-4 sm:p-6 lg:p-8 font-sans dark:bg-black">
      <div className="absolute top-4 right-4 z-10">
        <ModeToggle />
      </div>
      <div className="flex flex-col items-center gap-2 sm:gap-4 mb-4 sm:mb-6">
        <p className="heading-1 text-center">Daily Reminder</p>
        <p className="body-medium-bold text-center max-w-2xl px-4">
          Your app to boost NNN (Nonstop Ngoding November), so you won&apos;t
          scroll fesnuk. Be productive and happy vibe coding!
        </p>
      </div>
      <div className="flex-1 overflow-hidden">
        <Calendar />
      </div>
    </div>
  );
}
