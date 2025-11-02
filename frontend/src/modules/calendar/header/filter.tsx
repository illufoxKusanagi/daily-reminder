import { CheckIcon, Filter, RefreshCcw } from "lucide-react";
import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuItem,
  DropdownMenuTrigger,
} from "@/components/ui/dropdown-menu";
import { Separator } from "@/components/ui/separator";
import { Toggle } from "@/components/ui/toggle";
import { useCalendar } from "@/modules/calendar/contexts/calendar-context";
import type { TEventColor } from "@/modules/calendar/types";
import { ComponentProps } from "react";

export default function FilterEvents({ className }: ComponentProps<"div">) {
  const { selectedColors, filterEventsBySelectedColors, clearFilter } =
    useCalendar();

  const colorClasses: Record<TEventColor, string> = {
    blue: "bg-blue-600 dark:bg-blue-700",
    green: "bg-green-600 dark:bg-green-700",
    red: "bg-red-600 dark:bg-red-700",
    yellow: "bg-yellow-600 dark:bg-yellow-700",
    purple: "bg-purple-600 dark:bg-purple-700",
    orange: "bg-orange-600 dark:bg-orange-700",
  };

  const colors: TEventColor[] = [
    "blue",
    "green",
    "red",
    "yellow",
    "purple",
    "orange",
  ];

  return (
    <DropdownMenu>
      <DropdownMenuTrigger asChild>
        <Toggle
          variant="outline"
          className={`cursor-pointer w-fit ${className}`}
        >
          <Filter className="h-4 w-4" />
        </Toggle>
      </DropdownMenuTrigger>
      <DropdownMenuContent align="end" className="w-[150px]">
        {colors.map((color, index) => (
          <DropdownMenuItem
            key={index}
            className="flex items-center gap-2 cursor-pointer"
            onClick={(e) => {
              e.preventDefault();
              filterEventsBySelectedColors(color);
            }}
          >
            <div className={`size-3.5 rounded-full ${colorClasses[color]}`} />
            <span className="capitalize flex justify-center items-center gap-2">
              {color}
              <span>
                {selectedColors.includes(color) && (
                  <span className="text-blue-500">
                    <CheckIcon className="size-4" />
                  </span>
                )}
              </span>
            </span>
          </DropdownMenuItem>
        ))}
        <Separator className="my-2" />
        <DropdownMenuItem
          disabled={selectedColors.length === 0}
          className="flex gap-2 cursor-pointer"
          onClick={(e) => {
            e.preventDefault();
            clearFilter();
          }}
        >
          <RefreshCcw className="size-3.5" />
          Clear Filter
        </DropdownMenuItem>
      </DropdownMenuContent>
    </DropdownMenu>
  );
}
