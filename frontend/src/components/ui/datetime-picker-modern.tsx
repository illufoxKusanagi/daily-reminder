"use client";

import * as React from "react";
import { CalendarIcon, Clock } from "lucide-react";
import { format } from "date-fns";
import { cn } from "@/lib/utils";
import { Button } from "@/components/ui/button";
import { Calendar } from "@/components/ui/calendar";
import {
  Popover,
  PopoverContent,
  PopoverTrigger,
} from "@/components/ui/popover";
import { Label } from "@/components/ui/label";
import { Input } from "@/components/ui/input";

interface DateTimePickerProps {
  date: Date | undefined;
  setDate: (date: Date | undefined) => void;
  label?: string;
  placeholder?: string;
}

export function DateTimePickerModern({
  date,
  setDate,
  label,
  placeholder = "Pick a date and time",
}: DateTimePickerProps) {
  const [selectedDateTime, setSelectedDateTime] = React.useState<
    Date | undefined
  >(date);

  React.useEffect(() => {
    setSelectedDateTime(date);
  }, [date]);

  const handleDateSelect = (selectedDate: Date | undefined) => {
    if (selectedDate) {
      const newDateTime = new Date(selectedDate);
      if (selectedDateTime) {
        newDateTime.setHours(selectedDateTime.getHours());
        newDateTime.setMinutes(selectedDateTime.getMinutes());
      }
      setSelectedDateTime(newDateTime);
      setDate(newDateTime);
    }
  };

  const handleTimeChange = (type: "hours" | "minutes", value: string) => {
    if (!selectedDateTime) {
      const now = new Date();
      setSelectedDateTime(now);
    }

    const newDateTime = selectedDateTime
      ? new Date(selectedDateTime)
      : new Date();

    if (type === "hours") {
      const hours = parseInt(value) || 0;
      newDateTime.setHours(Math.min(23, Math.max(0, hours)));
    } else if (type === "minutes") {
      const minutes = parseInt(value) || 0;
      newDateTime.setMinutes(Math.min(59, Math.max(0, minutes)));
    }

    setSelectedDateTime(newDateTime);
    setDate(newDateTime);
  };

  const hours = selectedDateTime ? selectedDateTime.getHours() : 0;
  const minutes = selectedDateTime ? selectedDateTime.getMinutes() : 0;

  return (
    <div className="space-y-2">
      {label && <Label>{label}</Label>}
      <Popover>
        <PopoverTrigger asChild>
          <Button
            variant="outline"
            className={cn(
              "w-full justify-start text-left font-normal",
              !selectedDateTime && "text-muted-foreground"
            )}
          >
            <CalendarIcon className="mr-2 h-4 w-4" />
            {selectedDateTime ? (
              format(selectedDateTime, "PPP 'at' HH:mm")
            ) : (
              <span>{placeholder}</span>
            )}
          </Button>
        </PopoverTrigger>
        <PopoverContent className="w-auto p-0" align="start">
          <div className="p-3 border-b border-border">
            <Calendar
              mode="single"
              selected={selectedDateTime}
              onSelect={handleDateSelect}
              initialFocus
            />
          </div>
          <div className="p-3 space-y-2">
            <Label className="text-sm font-medium">Time</Label>
            <div className="flex items-center gap-2">
              <div className="flex items-center gap-1">
                <Clock className="h-4 w-4 text-muted-foreground" />
                <Input
                  type="number"
                  min={0}
                  max={23}
                  value={hours.toString().padStart(2, "0")}
                  onChange={(e) => handleTimeChange("hours", e.target.value)}
                  className="w-16 text-center"
                  placeholder="HH"
                />
              </div>
              <span className="text-xl font-semibold">:</span>
              <Input
                type="number"
                min={0}
                max={59}
                value={minutes.toString().padStart(2, "0")}
                onChange={(e) => handleTimeChange("minutes", e.target.value)}
                className="w-16 text-center"
                placeholder="MM"
              />
            </div>
            <div className="flex gap-2 pt-2">
              <Button
                size="sm"
                variant="outline"
                onClick={() => {
                  const now = new Date();
                  setSelectedDateTime(now);
                  setDate(now);
                }}
                className="flex-1"
              >
                Now
              </Button>
              <Button
                size="sm"
                variant="outline"
                onClick={() => {
                  setSelectedDateTime(undefined);
                  setDate(undefined);
                }}
                className="flex-1"
              >
                Clear
              </Button>
            </div>
          </div>
        </PopoverContent>
      </Popover>
    </div>
  );
}
