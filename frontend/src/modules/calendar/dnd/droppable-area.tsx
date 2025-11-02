import React, { useState, type ReactNode } from "react";
import { useDragDrop } from "@/modules/calendar/contexts/dnd-context";
import { cn } from "@/lib/utils";

interface DroppableAreaProps {
  date: Date;
  hour?: number;
  minute?: number;
  children: ReactNode;
  className?: string;
}

export function DroppableArea({
  date,
  hour,
  minute,
  children,
  className,
}: DroppableAreaProps) {
  const { handleEventDrop, isDragging } = useDragDrop();
  const [isDragOver, setIsDragOver] = useState(false);

  return (
    <div
      className={cn(
        className,
        isDragging && "drop-target",
        isDragOver && "bg-primary/10"
      )}
      onDragOver={(e) => {
        e.preventDefault();
        setIsDragOver(true);
      }}
      onDragLeave={(e) => {
        setIsDragOver(false);
      }}
      onDrop={(e) => {
        e.preventDefault();
        setIsDragOver(false);
        handleEventDrop(date, hour, minute);
      }}
    >
      {children}
    </div>
  );
}
